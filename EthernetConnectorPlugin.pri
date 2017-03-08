!contains( included_modules, $$PWD ) {
    included_modules += $$PWD
    QT += core gui widgets

    !include($$PWD/PluginInterface/UrgBenriPluginInterface.pri) {
            error("Unable to include Viewer Plugin Interface.")
    }

    !include($$PWD/ModeSwitcherWidget/ModeSwitcherWidget.pri) {
            error("Unable to include Mode Switcher Widget.")
    }

    !include($$PWD/QUrgLib/QUrgLib.pri) {
            error("Unable to include QUrg Library.")
    }

    !include($$PWD/EthernetConnectionWidget/EthernetConnectionWidget.pri) {
            error("Unable to include Ethernet Connection Widget.")
    }

    !include($$PWD/RangeViewWidget/RangeViewWidget.pri) {
            error("Unable to include Range View Widget.")
    }


    DEPENDPATH += $$PWD
    INCLUDEPATH += $$PWD

    SOURCES += \
            $$PWD/EthernetConnectorPlugin.cpp \
            $$PWD/EthernetConnectionCheck.cpp \
            $$PWD/EthernetDataConverter.cpp \
            $$PWD/EthernetSettingsForm.cpp

    HEADERS  += \
            $$PWD/EthernetConnectorPlugin.h \
            $$PWD/EthernetConnectionCheck.h \
            $$PWD/EthernetDataConverter.h \
            $$PWD/EthernetSettingsForm.h

    FORMS    += \
            $$PWD/EthernetConnectorPlugin.ui \
            $$PWD/EthernetSettingsForm.ui

    RESOURCES += \
            $$PWD/EthernetConnectorPlugin.qrc

    TRANSLATIONS = $$PWD/i18n/plugin_fr.ts \
            $$PWD/i18n/plugin_en.ts \
            $$PWD/i18n/plugin_ja.ts
}

