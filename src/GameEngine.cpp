#include "GameEngine.h"
#include <QTextStream>

GameEngine::Cell::Cell(int occupier, bool king_)
    : cellOccupier(occupier), king(king_)
{

}

int GameEngine::Cell::occupier() const
{
    return cellOccupier;
}

bool GameEngine::Cell::isEmpty() const
{
    return cellOccupier < 0;
}

bool GameEngine::Cell::isKing() const
{
    return king;
}

bool GameEngine::Cell::isDied() const
{
    return died;
}

bool GameEngine::Cell::isMovable() const
{
    return movable;
}

void GameEngine::Cell::setKing(bool isKing)
{
    king = isKing;
}

void GameEngine::Cell::setDied(bool isDied)
{
    died = isDied;
}

void GameEngine::Cell::setMovable(bool isMovable)
{
    movable = isMovable;
}

void GameEngine::Cell::setOccupier(int occupier, bool king_)
{
    cellOccupier = occupier;
    king = king_;
}

GameEngine::GameEngine(int role, int whoseTurn)
{
    reset(role, whoseTurn);
}

GameEngine::GameEngine(QString state)
{
    readState(std::move(state));
}

void GameEngine::readState(QString state)
{
    if (!state.isEmpty())
    {
        QTextStream in(&state);
        in >> me >> current;
        for (int i = 0; i < 10; ++i)
            for (int j = 0; j < 10; ++j)
            {
                int occupier = -1, king = -1;
                in >> occupier >> king;
                board.get(i, j).setOccupier(occupier, king);
            }
    }
}

void GameEngine::reset(int role, int whoseTurn)
{
    setRole(role);
    setWhoseTurn(whoseTurn);

    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
            board.get(i, j).setOccupier(-1);
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 10; ++j)
            if ((i + j) & 1)
                board.get(i, j).setOccupier(1 - role, false);
    for (int i = 6; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
            if ((i + j) & 1)
                board.get(i, j).setOccupier(role, false);
}

int GameEngine::role() const
{
    return me;
}

void GameEngine::setRole(int role)
{
    me = role;
    transpose();
}

int GameEngine::whoseTurn() const
{
    return current;
}

void GameEngine::setWhoseTurn(int whoseTurn)
{
    assert(whoseTurn == 0 || whoseTurn == 1);
    current = whoseTurn;
}

void GameEngine::setFinished()
{
    current = -1;
}

bool GameEngine::isFinished() const
{
    return current == -1;
}

void GameEngine::switchWhoseTurn()
{
    setWhoseTurn(current ^ 1);
}

constexpr int dx[4] = {-1, -1, 1, 1};
constexpr int dy[4] = {1, -1, 1, -1};

static bool isOutOfBoard(int x, int y)
{
    return x < 0 || y < 0 || x >= 10 || y >= 10;
}

void GameEngine::dfs(int x, int y, int len, int maxStep)
{
    if (len > longestEating)
    {
        longestEating = len;
        nextTemp->reset();
        nextTemp.get(path[0].x(), path[0].y()) = true;
    }
    else if (len == longestEating && path.size())
        nextTemp.get(path[0].x(), path[0].y()) = true;

    for (int k = 0; k < 4; ++k)
        for (int step = 1; step <= maxStep; ++step)
        {
            int xx = x + dx[k] * step;
            int yy = y + dy[k] * step;

            if (isOutOfBoard(xx, yy)) break;
            if (!board.get(xx, yy).isEmpty())
            {
                if (isMine(xx, yy)) break;
                if (vis.get(xx, yy) || board.get(xx, yy).isDied()) break;

                for (int nextStep = 1; nextStep <= maxStep; ++nextStep)
                {
                    int xxx = xx + dx[k] * nextStep;
                    int yyy = yy + dy[k] * nextStep;
                    if (isOutOfBoard(xxx, yyy)) break;
                    if (!board.get(xxx, yyy).isEmpty()) break;
                    if (vis.get(xxx, yyy)) break;
                    vis.get(xx, yy) = true;
                    path.push_back(QPoint(xxx, yyy));
                    dfs(xxx, yyy, len + 1, maxStep);
                    path.pop_back();
                    vis.get(xx, yy) = false;
                }
                break;
            }
    }
}

int GameEngine::lengthEating(int x, int y)
{
    longestEating = 0;
    path.clear();
    vis->reset();
    auto &cell = board.get(x, y);
    bool isKing = cell.isKing();
    int maxStep = isKing ? 9 : 1;
    int occupier = cell.occupier();
    cell.setOccupier(-1);
    dfs(x, y, 0, maxStep);
    cell.setOccupier(occupier, isKing);
    return longestEating;
}

bool GameEngine::updateMovable()
{
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
            board.get(i, j).setMovable(false);
    int length[10][10], maxLength = 0;
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
            if (isMine(i, j))
            {
                length[i][j] = lengthEating(i, j);
                maxLength = std::max(maxLength, length[i][j]);
            }
    bool hasNext = false;
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
            if (isMine(i, j) && length[i][j] == maxLength)
            {
                board.get(i, j).setMovable(true);
                hasNext = true;
            }
    return hasNext;
}

std::vector<QPoint> GameEngine::nextCells(int x, int y, bool mustJump)
{
    std::vector<QPoint> res;

    lengthEating(x, y);

    if (longestEating)
    {
        for (int i = 0; i < 10; ++i)
            for (int j = 0; j < 10; ++j)
                if (nextTemp.get(i, j))
                    res.push_back(QPoint(i, j));
    }

    if (!mustJump && !res.size())
    {
        int maxStep = board.get(x, y).isKing() ? 9 : 1;
        for (int k = 0; k < (board.get(x, y).isKing() ? 4 : 2); ++k)
            for (int step = 1; step <= maxStep; ++step)
            {
                int xx = x + dx[k] * step;
                int yy = y + dy[k] * step;
                if (isOutOfBoard(xx, yy)) break;
                if (!board.get(xx, yy).isEmpty()) break;
                res.push_back(QPoint(xx, yy));
            }
    }
    return res;
}

void GameEngine::transpose()
{
    for (int i = 0; i < 10 / 2; ++i)
        for (int j = 0; j < 10; ++j)
            std::swap(board.get(i, j), board.get(9 - i, 9 - j));
}

bool GameEngine::isMine(int x, int y) const
{
    return board.get(x, y).occupier() == role();
}

QString GameEngine::state(bool opponent) const
{
    if (!opponent)
    {
        QString res;
        QTextStream out(&res);
        out << me << " " << current << "\n";
        for (int i = 0; i < 10; ++i)
        {
            for (int j = 0; j < 10; ++j)
                out << (board.get(i, j).occupier()) << " " << (board.get(i, j).isKing()) << " ";
            out << "\n";
        }
        return res;
    }
    else
    {
        auto opponentEngine = *this;
        opponentEngine.setRole(1 - me);
        return opponentEngine.state(false);
    }

    return {};
}

bool GameEngine::move(QPoint S, QPoint E)
{
    auto hasDied = false;
    int dx = (S.x() < E.x()) ? 1 : -1;
    int dy = (S.y() < E.y()) ? 1 : -1;
    for (int x = S.x() + dx, y = S.y() + dy; x != E.x() && y != E.y(); x += dx, y += dy)
    {
        auto &cell = board.get(x, y);
        if (!cell.isEmpty())
        {
            cell.setDied(true);
            hasDied = true;
        }
    }

    auto &startCell = board.get(S.x(), S.y());
    auto &endCell = board.get(E.x(), E.y());

    endCell.setOccupier(startCell.occupier(), startCell.isKing());
    startCell.setOccupier(-1);

    return hasDied;
}

bool GameEngine::clearCorpses()
{
    bool hasDied = false;
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
        {
            auto &cell = board.get(i, j);
            if (cell.isDied())
            {
                cell.setOccupier(-1);
                cell.setDied(false);
                hasDied = true;
            }
        }
    return hasDied;
}

bool GameEngine::promote(int x, int y)
{
    auto &cell = board.get(x, y);
    if (cell.isKing())
        return false;

    int isMineKing = isMine(x, y);
    if ((x == 0 && isMineKing) ||
        (x == 9 && !isMineKing))
    {
        cell.setOccupier(cell.occupier(), true);
        return true;
    }

    return false;
}

bool GameEngine::applyMoveAchievements(QPoint lastMove)
{
    bool t1 = promote(lastMove.x(), lastMove.y());
    bool t2 = clearCorpses();
    return t1 || t2;
}
