#include "AIManager.h"
#include "GameEngine.h"
#include "Game.h"

#include <QTimer>

AIManager::AIManager(const GameEngine &gameEngine, Game *g, QObject *parent)
    : QObject(parent), engine(gameEngine), game(g)
{
    moveTimer = new QTimer(this);
    moveTimer->callOnTimeout(this, &AIManager::moveAI);
    connect(game, &Game::sendMessage, this, &AIManager::handleMessage);
}

void AIManager::handleMessage(QString message)
{
    QTextStream in(&message);
    QString operation;
    in >> operation;

    if (operation == "wait")
    {
        qInfo("Calculate AI move");
        calculatedMoves = calculateMoves();
        if (calculatedMoves.empty())
            return game->win();
        std::reverse(calculatedMoves.begin(), calculatedMoves.end());
        moveTimer->start(500);
    }
}

void AIManager::moveAI()
{
    auto move = calculatedMoves.pop_back_val();
    game->move(move.S, move.E, false);
    if (calculatedMoves.empty())
    {
        moveTimer->stop();
        game->endMove(false);
    }
}

vector<AIManager::Move> AIManager::calculateMoves() const
{
    auto gameEngineAI = engine;
    gameEngineAI.changeRole();
    bool hasNext = gameEngineAI.updateMovable();
    if (!hasNext)
        return {};

    SmallVector<Move, 10> moves;
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
        {
            auto &piece = gameEngineAI.board.get(i, j);
            if (piece.isMovable())
            {
                auto next = gameEngineAI.nextCells(i, j);
                for (auto E : next)
                    moves.push_back(Move{QPoint{i, j}, E});
            }
        }

    vector<Move> result;
    Move move = moves[rand() % moves.size()];
    QPoint S = move.S, E = move.E;
    auto makeMove = [&] {
        result.push_back(Move{QPoint(9 - S.x(), 9 - S.y()),
                              QPoint(9 - E.x(), 9 - E.y())});
        return gameEngineAI.move(S, E);
    };
    while (makeMove())
    {
        S = E;
        auto next = gameEngineAI.nextCells(S.x(), S.y(), true);
        if (next.empty())
            break;
        E = next.size() == 1 ? next.front() : next[rand() % next.size()];
    }

    return result;
}
