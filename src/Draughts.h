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

#ifndef DRAUGHTS_H
#define DRAUGHTS_H

#include "Common.h"
#include "Landing.h"
#include "Server.h"
#include "Client.h"
#include "Connection.h"
#include "GameEngine.h"
#include "Game.h"

class AIManager;

class Draughts : public QDialog
{
    Q_OBJECT
    
public:
    explicit Draughts(QWidget *parent = nullptr);
    
private slots:
    void createGameVsAI(const GameEngine &engine);
    void createGame(QString nickname, QString ip, int port, const GameEngine &engine);
    void joinGame(QString nickname, QString ip, int port);
    void handleMessage(QString message);
    void clientJoined(QString ip);
    void initGame();
    void startGame();
    void returnToHome();
    
private:      
    QStackedWidget *stackedWidget;
    Landing *landing;
    PendingMsg *pendingMsg;
    Server *server;
    Client *client;
    Connection *connection;
    AIManager *AI = nullptr;
    GameEngine gameEngine;
    Game *game;
    
    QString nickname[2], ip[2];

    enum class GameMode
    {
        versusAI, online
    };
    GameMode mode = GameMode::online;

    enum class Side
    {
        server, client
    };
    Side side;
};

#endif
