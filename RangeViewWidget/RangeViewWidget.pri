!contains( included_modules, $$PWD ) {
    included_modules += $$PWD
QT       += core gui

DEPENDPATH += "$$PWD/src"
INCLUDEPATH += "$$PWD/src"

SOURCES += $$PWD/src/RangeViewWidget.cpp

HEADERS  += $$PWD/src/RangeViewWidget.h

}
