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

#include "Generator.h"

Button* GeneratorSidebarButtons::renderButton(QString text)
{
    Button *button = new Button(text);
    button->setStyleSheet(button->styleSheet() + 
                          "font-size: 18px;"
                          "padding: 10px 0;");
    return button;
}

GeneratorSidebarButtons::GeneratorSidebarButtons(QWidget *parent) : 
    Widget(parent)
{
    buttonFirst = renderButton("First: Black");
    buttonMe = renderButton("Me: Black");
    buttonWhiteMan = renderButton("White Man");
    buttonWhiteKing = renderButton("White King");    
    buttonBlackMan = renderButton("Black Man");    
    buttonBlackKing = renderButton("Black King");    
    buttonErase = renderButton("Erase");
    buttonClear = renderButton("Clear");
    buttonImport = renderButton("Import");
    buttonExport = renderButton("Export");
    buttonDone = renderButton("Done");
    buttonStylePrimary = buttonFirst->styleSheet();
    buttonStyleHighlighted = buttonFirst->styleSheet() + QString("background: %1;").arg(Config::Colors::DANGEROUS);
    
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addStretch();
    layout->addWidget(buttonFirst);
    layout->addWidget(buttonMe);
    layout->addWidget(buttonBlackMan);
    layout->addWidget(buttonWhiteMan);    
    layout->addWidget(buttonBlackKing);
    layout->addWidget(buttonWhiteKing);
    layout->addWidget(buttonErase);
    layout->addWidget(buttonClear);
    layout->addWidget(buttonImport);
    layout->addWidget(buttonExport);
    layout->addWidget(buttonDone);
    layout->addStretch();
    layout->setContentsMargins(20, 4, 20, 4);
    layout->setSpacing(10);
    setLayout(layout);
    
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

GeneratorSidebar::GeneratorSidebar(QWidget *parent) : 
    Widget(parent)
{
    buttons = new GeneratorSidebarButtons();
    
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(buttons);
    
    layout->setSpacing(0);
    layout->setMargin(4);
    
    setLayout(layout);
    
    setObjectName("GeneratorSidebar");
    setStyleSheet("#GeneratorSidebar{"
                  "border: 4px solid " + Config::Colors::BORDER + ";"
                  "}");    
    setFixedWidth(200);      
}

Generator::Generator(QWidget *parent) : 
    QDialog(parent)
{
    board = new Board(gameEngine);
    sidebar = new GeneratorSidebar;
    
    button[0] = sidebar->buttons->buttonBlackMan;
    button[1] = sidebar->buttons->buttonBlackKing;
    button[2] = sidebar->buttons->buttonWhiteMan;
    button[3] = sidebar->buttons->buttonWhiteKing;
    button[4] = sidebar->buttons->buttonErase;
    
    connect(sidebar->buttons->buttonFirst, &Button::clicked, this, &Generator::clicked);
    connect(sidebar->buttons->buttonMe, &Button::clicked, this, &Generator::clicked);
    connect(sidebar->buttons->buttonWhiteMan, &Button::clicked, this, &Generator::clicked);
    connect(sidebar->buttons->buttonWhiteKing, &Button::clicked, this, &Generator::clicked);
    connect(sidebar->buttons->buttonBlackMan, &Button::clicked, this, &Generator::clicked);
    connect(sidebar->buttons->buttonBlackKing, &Button::clicked, this, &Generator::clicked);
    connect(sidebar->buttons->buttonErase, &Button::clicked, this, &Generator::clicked);
    connect(sidebar->buttons->buttonClear, &Button::clicked, this, &Generator::clicked);
    connect(sidebar->buttons->buttonImport, &Button::clicked, this, &Generator::clicked);
    connect(sidebar->buttons->buttonExport, &Button::clicked, this, &Generator::clicked);
    connect(sidebar->buttons->buttonDone, &Button::clicked, this, &Generator::clicked);
    
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
            connect(board->cell[i][j], &Cell::clicked, this, &Generator::clickCell);
    
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(board);
    layout->addWidget(sidebar);
    
    layout->setMargin(10);
    layout->setSpacing(10);
     
    setLayout(layout);
    
    setWindowFlags(Qt::Window 
                     | Qt::WindowSystemMenuHint
                     | Qt::WindowMinimizeButtonHint
                     | Qt::WindowCloseButtonHint); 
    setStyleSheet("QDialog { background: " + Config::Colors::BACKGROUND + "; }");   
    setFixedWidth(830);  
    setWindowTitle("Game Editor");
}

void Generator::reset(int role)
{
    gameEngine.reset(role);
    board->update();
}

void Generator::importData()
{
    QString file = QFileDialog::getOpenFileName(this);
    if (file == "") return;
    QFile f(file);
    if (!f.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this, "Can't read file", "Can't read file!");
        return;
    }

    gameEngine.readState(f.readAll());
    if (gameEngine.role() == 0)
        sidebar->buttons->buttonMe->setText("Me: Black");
    else
        sidebar->buttons->buttonMe->setText("Me: White");
    if (gameEngine.whoseTurn() == 0)
        sidebar->buttons->buttonFirst->setText("First: Black");
    else
        sidebar->buttons->buttonFirst->setText("First: White");
    board->update();

    f.close();
    QMessageBox::information(this, "Imported", "Imported!");
}

void Generator::exportData()
{
    QString file = QFileDialog::getSaveFileName(this);
    if (file == "") return;
    QFile f(file);
    if (!f.open(QIODevice::WriteOnly)) 
    {
        QMessageBox::critical(this, "Can't write file", "Can't write file!");
        return;
    }
    QTextStream out(&f);
    out << gameEngine.state();
    f.close();
    QMessageBox::information(this, "Exported", "Exported!");
}

void Generator::clicked(QString text)
{
    if (text == "First: White")
    {
        gameEngine.setWhoseTurn(0);
        sidebar->buttons->buttonFirst->setText("First: Black");
    }
    else if (text == "First: Black")
    {
        gameEngine.setWhoseTurn(1);
        sidebar->buttons->buttonFirst->setText("First: White");
    }
    else if (text == "Me: White")
    {
        gameEngine.setRole(0);
        sidebar->buttons->buttonMe->setText("Me: Black");
    }
    else if (text == "Me: Black")
    {
        gameEngine.setRole(1);
        sidebar->buttons->buttonMe->setText("Me: White");
    }
    else if (text == "Clear")
    {
        for (int i = 0; i < 10; ++i)
            for (int j = 0; j < 10; ++j)
                board->cell[i][j]->setOccupier(-1);
    }
    else if (text == "Import")
        importData();
    else if (text == "Export")
        exportData();
    else if (text == "Done")
        accept();
    else
    {
        for (int i = 0; i < 5; ++i)
            if (button[i]->text() == text)
            {
                currentBtn = i;
                button[i]->setStyleSheet(sidebar->buttons->buttonStyleHighlighted);
            }
            else
                button[i]->setStyleSheet(sidebar->buttons->buttonStylePrimary);
    }
    board->update();
}

void Generator::clickCell(int x, int y)
{
    if ((x + y) % 2 == 0) return;
    if (currentBtn == 4)
    {
        board->cell[x][y]->setOccupier(-1);
    }
    else
    {
        int color = currentBtn < 2 ? 0 : 1;
        bool king = currentBtn % 2;
        board->cell[x][y]->setOccupier(color, king);
    }
}

const GameEngine &Generator::engine() const
{
    return gameEngine;
}
