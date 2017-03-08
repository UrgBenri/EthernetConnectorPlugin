!contains( included_modules, $$PWD ) {
    included_modules += $$PWD
    QT += core gui

    DEFINES += MODE_SWITCHER
    DEPENDPATH += $$PWD
    INCLUDEPATH += $$PWD

    SOURCES += \
        $$PWD/ModeSwitcherWidget.cpp \
        $$PWD/ModeSwitchButton.cpp

    HEADERS  += \
        $$PWD/ModeSwitcherWidget.h \
        $$PWD/ModeSwitchButton.h

    FORMS += \
        $$PWD/ModeSwitcherWidget.ui

    RESOURCES += \
        $$PWD/ModeSwitcherWidget.qrc
}

