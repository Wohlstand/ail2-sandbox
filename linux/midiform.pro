TEMPLATE = app
CONFIG += console
CONFIG -= qt

LIBS += -lncurses

QMAKE_CFLAGS += -std=c90 -Wextra

SOURCES += midiform.c
