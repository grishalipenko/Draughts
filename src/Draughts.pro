#-------------------------------------------------
#
# Project created by QtCreator 2017-09-05T10:03:57
#
#-------------------------------------------------

QT       += core gui network multimedia
CONFIG	 += c++17
CONFIG	 += sanitizer sanitize_address

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Draughts
TEMPLATE = app


SOURCES += main.cpp \
    AIManager.cpp \
    Common.cpp \
    GameEngine.cpp \
    Landing.cpp \
    Draughts.cpp \
    CreateGameDialog.cpp \
    Server.cpp \
    JoinGameDialog.cpp \
    Client.cpp \
    Connection.cpp \
    Game.cpp \
    Generator.cpp

HEADERS  += \
    AIManager.h \
    Common.h \
    Config.h \
    GameEngine.h \
    Landing.h \
    Draughts.h \
    CreateGameDialog.h \
    Server.h \
    JoinGameDialog.h \
    Client.h \
    Connection.h \
    Game.h \
    Generator.h \
    Vector.h \
    utils/SmallVector.h

FORMS    += \
    CreateGameDialog.ui \
    JoinGameDialog.ui

RESOURCES += \
    Draughts.qrc
