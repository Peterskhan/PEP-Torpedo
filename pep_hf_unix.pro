TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    game_control.c \
    game_statistics.c \
    command_args.c

HEADERS += \
    game_control.h \
    game_statistics.h \
    command_args.h

LIBS += \
    -pthread
