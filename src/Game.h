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

#ifndef GAME_H
#define GAME_H

#include "Common.h"

class GameEngine;

class Cell : public QLabel
{
    Q_OBJECT

public:
    explicit Cell(GameEngine &engine, QColor background, int x, int y, QWidget *parent = nullptr);
    void setOccupier(int occupier, bool king = false);
    void setFocused(bool focused);
    void setHighlighted(bool highlighted);
    bool isHighlighted() const;
    
signals:
    void clicked(int x, int y);    
    
private:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *event);

    GameEngine &gameEngine;
    QColor background;
    int x, y;
    bool focused, highlighted;
};

class Board : public Widget
{
    Q_OBJECT

public:
    explicit Board(GameEngine &engine, QWidget *parent = nullptr);
    
private:
    Cell *cell[10][10];
    
    friend class Game;
    friend class Generator;
};

class GameSidebarPlayerStatus : public QLabel
{
    Q_OBJECT

public:
    explicit GameSidebarPlayerStatus(int role, QWidget *parent = nullptr);
    void setActive(bool active);    
    void setWinner();
    
private:
    void paintEvent(QPaintEvent *);    
    
    QPen pen;
    QBrush brush;
    QPixmap winner;
    
    friend class Game;
};

class GameSidebarPlayer : public Widget
{
    Q_OBJECT

public:
    explicit GameSidebarPlayer(QString name, QString ip, int role, QWidget *parent = nullptr);
    
private:
    QLabel* renderText(QString text);
    
    GameSidebarPlayerStatus *status;    
    QLabel *name, *ip;
    
    friend class Game;
};

class GameSidebarButtons : public Widget
{
    Q_OBJECT

public:
    explicit GameSidebarButtons(QWidget *parent = nullptr);
    
private:
    Button* renderButton(QString text);
    
    Button *buttonRequestDraw, *buttonResign, *buttonSound;    
    
    friend class Game;
};

class GameSidebar : public Widget
{
    Q_OBJECT
    
public:    
    explicit GameSidebar(QString name0, QString ip0, int role0, QString name1, QString ip1, int role1, QWidget *parent = nullptr);
    
private:
    GameSidebarPlayer *player[2];  
    GameSidebarButtons *buttons;
    
    friend class Game;
};

class Game : public QDialog
{
    Q_OBJECT
    
public:
    explicit Game(GameEngine &engine, QString name0, QString ip0, QString name1, QString ip1, QWidget *parent = nullptr);
    void start();
    void lose(QString message = "");
    void win(QString message = "Congratulations!");  
    void draw();
    void move(bool informOpponent = true);  
    bool move(QPoint S, QPoint E, bool informOpponent = false);
    
private slots:
    void clickCell(int x, int y); 
    void requestDraw();
    bool resign();
    void switchSound(QString text);
    
signals:
    void sendMessage(QString message); 
    void checkMessages();
    
private:
    void closeEvent(QCloseEvent *event);
    void setFocus(int x, int y, bool mustJump = false);
    void switchCurrent();
    void playSound(QSoundEffect *sound);
    QSoundEffect* renderSound(QString url);
    
    GameEngine &gameEngine;

    Board *board;
    GameSidebar *gameSidebar;
    QPoint focus, lastMove;
    bool focusLocked;
    
    QSoundEffect *soundMove, *soundEat, *soundWin, *soundLose;
    bool sound;
};

#endif
