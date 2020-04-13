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

#include "Draughts.h"
#include "AIManager.h"

Draughts::Draughts(QWidget *parent) : 
    QDialog(parent)
{
    qInstallMessageHandler(MsgHandler::handler);  
    
    landing = new Landing; 
    pendingMsg = new PendingMsg;
    
    connect(landing, &Landing::createGameVsAI, this, &Draughts::createGameVsAI);
    connect(landing, &Landing::createGame, this, &Draughts::createGame);
    connect(landing, &Landing::joinGame, this, &Draughts::joinGame);
    connect(pendingMsg, &PendingMsg::canceled, this, &Draughts::returnToHome);
    
    stackedWidget = new QStackedWidget();
    stackedWidget->addWidget(landing);
    stackedWidget->addWidget(pendingMsg);
    
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(stackedWidget);
    layout->setMargin(0);
    setLayout(layout);
    
    stackedWidget->setCurrentIndex(0);
    
    setMinimumHeight(700);
    setMinimumWidth(500);
    setWindowTitle("Draughts");
    setStyleSheet("QDialog { background: " + Config::Colors::BACKGROUND + "; }");       
}

void Draughts::createGameVsAI(const GameEngine &engine)
{
    mode = GameMode::versusAI;
    gameEngine = engine;
    nickname[0] = "You";
    nickname[1] = "AI";
    ip[0] = ip[1] = QString{};
    startGame();
}

void Draughts::returnToHome()
{
    switch (side)
    {
    case Side::server:
        server->close();
        break;
    case Side::client:
        client->close();
        break;
    }
    stackedWidget->setCurrentIndex(0);
}

void Draughts::createGame(QString nickname, QString ip, int port, const GameEngine &engine)
{
    mode = GameMode::online;
    this->ip[0] = ip;
    this->nickname[0] = nickname;
    gameEngine = engine;
    side = Side::server;
    server = new Server(ip, port);
    connect(server, &Server::connected, this, &Draughts::clientJoined);
    
    pendingMsg->setIp(ip);
    pendingMsg->setPort(port);    
    stackedWidget->setCurrentIndex(1);    
}

void Draughts::joinGame(QString nickname, QString ip, int port)
{
    mode = GameMode::online;
    this->nickname[0] = nickname;
    this->ip[1] = ip;
    side = Side::client;
    client = new Client(ip, port);
    connection = new Connection(client->socket());
    connect(connection, &Connection::receivedMessage, this, &Draughts::handleMessage);
    
    pendingMsg->setIp(ip);
    pendingMsg->setPort(port);
    stackedWidget->setCurrentIndex(1);
}

void Draughts::clientJoined(QString ip)
{
    this->ip[1] = ip;
    connection = new Connection(server->socket());  
    qInfo("Client joined: %s", server->socket()->peerAddress().toString().toStdString().c_str());
    connect(connection, &Connection::receivedMessage, this, &Draughts::handleMessage);
    connection->sendMessage(QString("server %1 %2").arg(nickname[0]).arg(this->ip[1]));
}

void Draughts::handleMessage(QString message)
{
    QTextStream in(&message);
    QString operation;
    
    while (!in.atEnd())
    {
        in >> operation;
        if (operation == "client")
        {
            in >> nickname[1];
            qInfo("Client's nickname is %s", nickname[1].toStdString().c_str());
            initGame();
        }
        else if (operation == "server")
        {
            in >> nickname[1] >> ip[0];
            qInfo("Server's nickname is %s", nickname[1].toStdString().c_str());
            connection->sendMessage("client " + nickname[0]);       
        }
        else if (operation == "start")
        {
            gameEngine.readState(in.readAll());
            startGame();
        }
        else if (operation == "move")
        {
            int sx, sy, ex, ey;
            in >> sx >> sy >> ex >> ey;
            game->move(QPoint(9 - sx, 9 - sy), QPoint(9 - ex, 9 - ey));
        }
        else if (operation == "endMove")
            game->endMove(false);
        else if (operation == "finish")
            game->win();
        else if (operation == "requestDraw")
        {
            int ret = QMessageBox::information(this, "Draw Request", "Your opponent has requested a draw.<br>Would you accept it?", QMessageBox::Yes | QMessageBox::No);
            if (ret == QMessageBox::Yes)
            {
                connection->sendMessage("draw accepted");
                game->draw();
            }
            else
                connection->sendMessage("draw rejected");
        }
        else if (operation == "draw")
        {
            QString ret;
            in >> ret;
            if (ret == "accepted")
                game->draw();
            else
                QMessageBox::information(this, "Draw Request", "Your draw request has been rejected by your opponent.");
        }
        else if (operation == "resign")
            game->win("Your opponent has resigned.");
    }
    connection->checkReadable();
}

void Draughts::startGame()
{
    hide();
    game = new Game(gameEngine, nickname[1], ip[1], nickname[0], ip[0]);

    QString title = "Draughts";
    switch (mode)
    {
    case GameMode::online:
        {
            connect(game, &Game::sendMessage, connection, &Connection::sendMessage);
            connect(game, &Game::checkMessages, connection, &Connection::checkReadable);
            title += QString(" [%1]").arg(nickname[0]);
            break;
        }
    case GameMode::versusAI:
        {
            AI = new AIManager(gameEngine, game, this);
            break;
        }
    }

    game->setWindowTitle(title);
    game->start();
}

void Draughts::initGame()
{
    hide();    
    connection->sendMessage("start " + gameEngine.state(true));
    startGame();
}
