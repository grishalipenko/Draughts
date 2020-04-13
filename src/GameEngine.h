#pragma once

#include <array>
#include <bitset>
#include <QString>
#include <QPoint>
#include "Vector.h"

class GameEngine
{
public:
    template<typename T, typename Dummy = void> // workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=85282
    class Board
    {
        std::array<T, 10 * 10> container;
    public:
        const T &get(int i, int j) const
        {
            return container[i * 10 + j];
        }

        T &get(int i, int j)
        {
            return container[i * 10 + j];
        }

        auto *operator->()
        {
            return &container;
        }
    };

    template<typename Dummy>
    class Board<bool, Dummy>
    {
        std::bitset<10 * 10> container;
    public:
        auto get(int i, int j)
        {
            return container[i * 10 + j];
        }

        auto *operator->()
        {
            return &container;
        }
    };

    class Cell
    {
        int cellOccupier = -1; // empty=-1, dark=0, light=1
        bool movable = false;
        bool died = false;
        bool king = false;

    public:
        explicit Cell (int occupier = -1, bool king_ = false);

        int occupier() const;
        bool isEmpty() const;
        bool isKing() const;
        bool isDied() const;
        bool isMovable() const;

        void setKing(bool isKing);
        void setDied(bool isDied);
        void setMovable(bool isMovable);
        void setOccupier(int occupier, bool king_ = false);
    };
    Board<Cell> board;

    explicit GameEngine(int role = 0, int whoseTurn = 0);
    explicit GameEngine(QString state);

    void reset(int role = 0, int whoseTurn = 0);

    int role() const;
    void setRole(int role);
    void changeRole();
    bool isMine(int x, int y) const;
    int whoseTurn() const;
    void setWhoseTurn(int whoseTurn);
    void switchWhoseTurn();
    bool isMyTurn() const;
    void setFinished();
    bool isFinished() const;
    bool updateMovable(); // returns true if has next move
    vector<QPoint> nextCells(int x, int y, bool mustJump = false);
    bool move(QPoint S, QPoint E); // returns true if has died
    bool applyMoveAchievements(QPoint lastMove); // returns true if has some achievement

    QString state(bool opponent = false) const;
    void readState(QString state);
    void transpose();

private:
    int me = -1, current = -1;

    int longestEating = 0;
    Board<bool> nextTemp, vis;
    vector<QPoint> path;

    int lengthEating(int x, int y);
    void dfs(int x, int y, int len, int maxStep);
    bool clearCorpses();
    bool promote(int x, int y);
};
