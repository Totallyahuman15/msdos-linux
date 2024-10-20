TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += link_pkgconfig
PKGCONFIG += ncurses

SOURCES += \
        main.cpp
