#pragma once

#include <QObject>
#include <QPoint>
#include "Vector.h"

class QTimer;
class GameEngine;
class Game;

class AIManager : public QObject
{
    struct Move
    {
        QPoint S, E;
    };

    const GameEngine &engine;
    Game *game = nullptr;
    QTimer *moveTimer = nullptr;
    vector<Move> calculatedMoves;

public:
    AIManager(const GameEngine &gameEngine, Game *g, QObject *parent = nullptr);
    void moveAI();

private slots:
    void handleMessage(QString message);

private:
    vector<Move> calculateMoves() const;
};

