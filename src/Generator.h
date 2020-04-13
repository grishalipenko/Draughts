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

#ifndef GENERATOR_H
#define GENERATOR_H

#include "Common.h"
#include "GameEngine.h"
#include "Game.h"

class GeneratorSidebarButtons : public Widget
{
    Q_OBJECT

public:
    explicit GeneratorSidebarButtons(QWidget *parent = nullptr);
    
private:
    Button* renderButton(QString text);
    
    Button *buttonMe, *buttonFirst, *buttonWhiteMan, *buttonWhiteKing, *buttonBlackMan, *buttonBlackKing,
           *buttonEraser, *buttonImport, *buttonExport, *buttonErase, *buttonClear, *buttonDone;
    QString buttonStylePrimary, buttonStyleHighlighted;
    
    friend class Generator;
};

class GeneratorSidebar : public Widget
{
    Q_OBJECT
    
public:
    explicit GeneratorSidebar(QWidget *parent = nullptr);
    
private:
    GeneratorSidebarButtons *buttons;   
    
    friend class Generator;
};

class Generator : public QDialog
{
    Q_OBJECT
    
public:    
    explicit Generator(QWidget *parent = nullptr);
    const GameEngine &engine() const;
    void reset(int role = 0);
    
private slots:
    void clickCell(int x, int y);
    void clicked(QString text);
    
private:
    void importData();
    void exportData();
    
    int currentBtn = 4;
    GameEngine gameEngine;
    Board *board;
    GeneratorSidebar *sidebar;  
    Button *button[5];
};

#endif
