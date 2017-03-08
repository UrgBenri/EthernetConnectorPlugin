!contains( included_modules, $$PWD ) {
    included_modules += $$PWD
    QT += core gui

    DEPENDPATH += $$PWD/src
    INCLUDEPATH += $$PWD/src

    SOURCES += $$PWD/src/EthernetConnectionWidget.cpp

    HEADERS  += $$PWD/src/EthernetConnectionWidget.h

    FORMS += $$PWD/src/EthernetConnectionWidgetForm.ui

    RESOURCES += $$PWD/EthernetConnectionWidget.qrc
}
