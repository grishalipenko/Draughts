/***********************************************************************
    Draughts

    Copyright (c) 2017 Zhouxing Shi <zhouxingshichn@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

**********************************************************************/ 

#include "GameEngine.h"
#include "Game.h"

Cell::Cell(GameEngine &engine, QColor background, int x, int y, QWidget *parent) :
    QLabel(parent), gameEngine(engine)
{
    this->background = background;
    this->x = x;
    this->y = y;
    this->focused = false;
    this->highlighted = false;
}

void Cell::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(highlighted ? Config::Colors::CELL_NEXT : background));
    painter.drawRect(0, 0, width(), height());

    auto &cell = gameEngine.board.get(x, y);
    if (!cell.isEmpty()) // draw piece
    {
        const int margin = 7;
        const int stroke = 5;
        if (focused)
            painter.setPen(QPen(Config::Colors::PIECE_FOCUSED, stroke));
        auto occupier = cell.occupier();
        if (occupier)
            painter.setBrush(QBrush(Config::Colors::PIECE_LIGHT));
        else
            painter.setBrush(QBrush(Config::Colors::PIECE_DARK));
        painter.drawEllipse(margin, margin, width() - margin * 2, height() - margin * 2);
        if (cell.isKing())
        {
            const int margin = 14;
            painter.drawPixmap(margin, margin, width() - margin * 2, height() - margin * 2,
                               occupier ? QPixmap(":/icons/king-light.png") : QPixmap(":/icons/king-dark.png"));
        }
    }
}

void Cell::setOccupier(int occupier, bool king)
{
    gameEngine.board.get(x, y) = GameEngine::Cell{occupier, king};
    this->focused = this->highlighted = false;
    update();
}

void Cell::setFocused(bool focused)
{
    this->focused = focused;
    update();
}

void Cell::setHighlighted(bool highlighted)
{
    this->highlighted = highlighted;
    update();
}

bool Cell::isHighlighted() const
{
    return highlighted;
}

void Cell::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        emit clicked(x, y);
}

Board::Board(GameEngine &engine, QWidget *parent) :
    Widget(parent)
{
    QGridLayout *layout = new QGridLayout;    
    
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
        {
            auto background = ((i + j) % 2) ? Config::Colors::CELL_DARK : Config::Colors::CELL_LIGHT;
            cell[i][j] = new Cell(engine, background, i, j);
            layout->addWidget(cell[i][j], i, j);
        }
                
    layout->setSpacing(0);
    layout->setMargin(4);
    setLayout(layout);
    
    setStyleSheet("border: 4px solid " + Config::Colors::BORDER + ";");
    
    setFixedWidth(600);
    setFixedHeight(600);
}

GameSidebarPlayerStatus::GameSidebarPlayerStatus(int role, QWidget *parent) :
    QLabel(parent)
{
    pen = Qt::NoPen;
    if (role == 0)
        brush = QBrush(Config::Colors::PIECE_DARK);
    else
        brush = QBrush(Config::Colors::PIECE_LIGHT);
    setFixedHeight(100);  
}

void GameSidebarPlayerStatus::setActive(bool active)
{
    if (!active)
        pen = Qt::NoPen;
    else
        pen = QPen(Config::Colors::PIECE_FOCUSED, 5);
    update();
}

void GameSidebarPlayerStatus::setWinner()
{        
    if (brush == QBrush(Config::Colors::PIECE_DARK))
        winner = QPixmap(":/icons/king-dark.png");
    else
        winner = QPixmap(":/icons/king-light.png");
    update();
}

void GameSidebarPlayerStatus::paintEvent(QPaintEvent *)
{
    const int r = 30;
    const int rWinner = 20;  
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);    
    painter.setBrush(brush);
    painter.setPen(pen);
    painter.drawEllipse(QPoint(width() / 2, height() / 2), r, r);
    painter.drawPixmap(width() / 2 - rWinner, height() / 2 - rWinner, 2 * rWinner, 2 * rWinner, winner);
}

QLabel* GameSidebarPlayer::renderText(QString text)
{
    QLabel *widget = new QLabel(text);
    widget->setAlignment(Qt::AlignCenter);
    widget->setStyleSheet("color: white;");
    return widget;
}

GameSidebarPlayer::GameSidebarPlayer(QString name, QString ip, int role, QWidget *parent) :
    Widget(parent)
{
    status = new GameSidebarPlayerStatus(role);
    this->name = renderText(name);
    this->ip = renderText(ip.isEmpty() ? ip : QString("IP: %1").arg(ip));
    
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(status);
    layout->addWidget(this->name);
    layout->addWidget(this->ip);
    
    layout->setSpacing(0);
    layout->setMargin(4);
    
    setLayout(layout);
    
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setObjectName("GameSidebarPlayer");
    setStyleSheet("background: " + Config::Colors::PRIMARY + ";");
}

Button* GameSidebarButtons::renderButton(QString text)
{
    Button *button = new Button(text);
    button->setStyleSheet(button->styleSheet() + 
                          "font-size: 18px;"
                          "padding: 10px 0;");
    return button;
}

GameSidebarButtons::GameSidebarButtons(QWidget *parent) : 
    Widget(parent)
{
    buttonRequestDraw = renderButton("Request Draw");
    buttonResign = renderButton("Resign");
    buttonSound = renderButton("Sound: On");
    
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addStretch();
    layout->addWidget(buttonRequestDraw);
    layout->addWidget(buttonResign);
    layout->addWidget(buttonSound);
    layout->addStretch();
    layout->setContentsMargins(20, 4, 20, 4);
    layout->setSpacing(10);
    setLayout(layout);
    
    setObjectName("GameSidebarButtons");
    setStyleSheet("#GameSidebarButtons{"
                  "border-top: 4px solid " + Config::Colors::BORDER + ";" +
                  "border-bottom: 4px solid " + Config::Colors::BORDER + ";"
                  "}");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

GameSidebar::GameSidebar(QString name0, QString ip0, int role0, QString name1, QString ip1, int role1, QWidget *parent) :
    Widget(parent)
{
    player[0] = new GameSidebarPlayer(name0, ip0, role0);
    player[1] = new GameSidebarPlayer(name1, ip1, role1);
    buttons = new GameSidebarButtons();
    
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(player[0]);
    layout->addWidget(buttons);
    layout->addWidget(player[1]);
    
    layout->setSpacing(0);
    layout->setMargin(4);
    
    setLayout(layout);
    
    setObjectName("GameSidebar");
    setStyleSheet("#GameSidebar{"
                  "border: 4px solid " + Config::Colors::BORDER + ";"
                  "}");    
    setFixedWidth(200);  
}

Game::Game(GameEngine &engine, QString name0, QString ip0, QString name1, QString ip1, QWidget *parent) :
    QDialog(parent), gameEngine(engine)
{
    board = new Board(gameEngine);
    int role = gameEngine.role();
    gameSidebar = new GameSidebar(name0, ip0, 1 - role, name1, ip1, role);
    
    sound = true;
    soundMove = renderSound(":/sounds/move.wav");
    soundEat = renderSound(":/sounds/eat.wav");
    soundWin = renderSound(":/sounds/win.wav");
    soundLose = renderSound(":/sounds/lose.wav");
    
    connect(gameSidebar->buttons->buttonRequestDraw, &Button::clicked, this, &Game::requestDraw);
    connect(gameSidebar->buttons->buttonResign, &Button::clicked, this, &Game::resign);
    connect(gameSidebar->buttons->buttonSound, &Button::clicked, this, &Game::switchSound);
    
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
            connect(board->cell[i][j], &Cell::clicked, this, &Game::clickCell);
    
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(board);
    layout->addWidget(gameSidebar);
    
    layout->setMargin(10);
    layout->setSpacing(10);
     
    setLayout(layout);
    
    setWindowFlags(Qt::Window 
                     | Qt::WindowSystemMenuHint
                     | Qt::WindowMinimizeButtonHint
                     | Qt::WindowCloseButtonHint); 
    setStyleSheet("QDialog { background: " + Config::Colors::BACKGROUND + "; }");   
    setFixedWidth(830);
}

void Game::start()
{
    qInfo("Game started: role = %d", gameEngine.role());

    gameEngine.switchWhoseTurn();
    focus = QPoint(-1, -1);
    focusLocked = false;
    switchCurrent();
    show();
}

void Game::setFocus(int x, int y, bool mustJump)
{
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
            board->cell[i][j]->setFocused(false);    
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
            board->cell[i][j]->setHighlighted(false);
    if (x != -1)
    {
        auto next = gameEngine.nextCells(x, y, mustJump);
        int nextSize = next.size();
        for (int i = 0; i < nextSize; ++i)
            board->cell[next[i].x()][next[i].y()]->setHighlighted(true);
        if (mustJump && !nextSize)
        {
            focus = QPoint(-1, -1);
            return;
        }
        else
            board->cell[x][y]->setFocused(true);
                
    }
    focus = QPoint(x, y);
}

void Game::clickCell(int x, int y)
{
    if (!gameEngine.isMyTurn()) return;
    const auto &cell = gameEngine.board.get(x, y);
    if (!cell.isEmpty())
    {
        if (!gameEngine.isMine(x, y)) return;
        if (!focusLocked)
        {
            if (cell.isMovable())
                setFocus(x, y);
            else
                setFocus(-1, -1);
        }
    }
    else
    {
        if (focus == QPoint(-1, -1)) return;
        if (!board->cell[x][y]->isHighlighted())
        {
            if (!focusLocked)
                setFocus(-1, -1);
        }
        else
        {
            auto hasDied = move(focus, QPoint(x, y), true);
            if (!hasDied)
            {
                setFocus(-1, -1);
                endMove();
            }
            else
            {
                setFocus(x, y, true);
                if (focus == QPoint(-1, -1))
                    endMove();
                else
                {
                    playSound(soundMove);
                    focusLocked = true;
                }
            }
        }
    }
}

bool Game::move(QPoint S, QPoint E, bool informOpponent)
{
    bool hasDied = gameEngine.move(S, E);
    board->update();
    
    lastMove = E;
    
    if (!informOpponent)
        playSound(soundMove);    
    else
    {
        QString message;
        QTextStream out(&message);
        out << "move " << S.x() << " " << S.y() << " " << E.x() << " " << E.y() << "\n";
        emit sendMessage(message);
    }

    return hasDied;
}

void Game::endMove(bool informOpponent)
{
    bool hasAchievements = gameEngine.applyMoveAchievements(lastMove);
    board->update();
    if (hasAchievements)
    {
        if (informOpponent)
            playSound(soundEat);
    }
    else
    {
        if (informOpponent)
            playSound(soundMove);
    }
    switchCurrent();
    if (informOpponent)
        emit sendMessage("endMove");
}

void Game::switchCurrent()
{
    gameEngine.switchWhoseTurn();
    focusLocked = false;
    for (int i = 0; i < 2; ++i)
    {
        bool active = gameEngine.whoseTurn() ^ i ^ gameEngine.role();
        gameSidebar->player[i]->status->setActive(active);
    }
    auto hasNext = gameEngine.updateMovable();
    if (!hasNext)
        lose();

    if (!gameEngine.isMyTurn())
        emit sendMessage("wait");
}

void Game::lose(QString message)
{
    if (gameEngine.isFinished()) return;
    gameEngine.setFinished();
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
        {
            board->cell[i][j]->setFocused(false);
            board->cell[i][j]->setHighlighted(false);
        }    
    emit sendMessage("finish");    
    for (int k = 0; k < 2; ++k)
        gameSidebar->player[k]->status->setActive(false);
    gameSidebar->player[0]->status->setWinner();    
    playSound(soundLose);
    QMessageBox::information(this, "You lose", QString("<h2>You lose.</h2><p>%1</p>").arg(message));
}

void Game::win(QString message)
{
    if (gameEngine.isFinished()) return;
    gameEngine.setFinished();
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
        {
            board->cell[i][j]->setFocused(false);
            board->cell[i][j]->setHighlighted(false);
        }    
    for (int k = 0; k < 2; ++k)
        gameSidebar->player[k]->status->setActive(false);
    gameSidebar->player[1]->status->setWinner();
    playSound(soundWin);
    QMessageBox::information(this, "You win", QString("<h2>You win!</h2><p>%1</p>").arg(message));
}

void Game::draw()
{
    for (int i = 0; i < 2; ++i)
    {
        gameSidebar->player[i]->status->setActive(false);
        gameSidebar->player[i]->status->setWinner();
    }
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
        {
            board->cell[i][j]->setFocused(false);
            board->cell[i][j]->setHighlighted(false);
        }
    gameEngine.setFinished();
    QMessageBox::information(this, "Draw", "<h2>Draw.</h2>");    
}

void Game::requestDraw()
{
    if (gameEngine.isFinished()) return;
    if (gameEngine.isMyTurn())
    {
        QMessageBox::information(this, "Can't Draw", "Please finish your operation first.");
        return;
    }
    int ret = QMessageBox::warning(this, "Request Draw", "Do you really want to request draw?", QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes)
    {
        QMessageBox::information(this, "Request Draw", "Draw request sent.");
        emit sendMessage("requestDraw");        
    }
    emit checkMessages();
}

bool Game::resign()
{
    if (gameEngine.isFinished()) return true;
    int ret = QMessageBox::warning(this, "Resign", "Do you really want to resign?", QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes)
    {
        emit sendMessage("resign");        
        lose("You've resigned.");
        return true;
    }
    else return false;
}

void Game::switchSound(QString text)
{
    if (text == "Sound: On")
    {
        sound = false;
        gameSidebar->buttons->buttonSound->setText("Sound: Off");
    }
    else
    {
        sound = true;
        gameSidebar->buttons->buttonSound->setText("Sound: On");
    }
}

void Game::playSound(QSoundEffect *sound)
{
    if (this->sound)
        sound->play();
}

QSoundEffect* Game::renderSound(QString url)
{
    QSoundEffect* sound = new QSoundEffect;
    sound->setSource(QUrl::fromLocalFile(url));    
    return sound;
}

void Game::closeEvent(QCloseEvent *event)
{
    if (resign())
        event->accept();
    else
        event->ignore();
}
