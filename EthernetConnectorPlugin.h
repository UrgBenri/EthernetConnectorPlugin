/*
	This file is part of the UrgBenri application.

	Copyright (c) 2016 Mehrez Kristou.
	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

	2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Please contact kristou@hokuyo-aut.jp for more details.

*/

#ifndef ETHERNETCOMMUNICATORWIDGET_H
#define ETHERNETCOMMUNICATORWIDGET_H

#define USE_DEVICE_NET

#include <QWidget>
#include <QTimer>
#include <QTime>
#include <ctime>
#include <QtPlugin>
#include <QTranslator>
#include <QPushButton>

#include "GeneralPluginInterface.h"
#include "ConnectorPluginInterface.h"
#include "HelperPluginInterface.h"

#include "EthernetConnectionWidget.h"
#include "RangeViewWidget.h"

#include "EthernetConnectionCheck.h"
#include "EthernetSettingsForm.h"

#include "UrgDevice.h"

#include "Thread.h"

#ifdef USE_DEVICE_NET
#include "TcpDevice.h"
#else
#include "TcpipSocket.h"
#endif

using namespace std;
using namespace qrk;

namespace Ui
{
class EthernetConnectorPlugin;
}

class EthernetConnectorPlugin : public ConnectorPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(ConnectorPluginInterface)
    Q_PLUGIN_METADATA(IID "org.kristou.UrgBenri.EthernetConnectorPlugin")
    PLUGIN_FACTORY(EthernetConnectorPlugin)
public:
    explicit EthernetConnectorPlugin(QWidget* parent = 0);
    virtual ~EthernetConnectorPlugin();

    bool isConnected() { return m_connected;}
    bool isStarted() { return m_started;}
    bool isPaused() { return m_paused;}
    static int receivingThreadProcess(void* args);
    void setLengthData(const SensorDataArray &l_ranges, const SensorDataArray &l_levels, long l_timeStamp);

    QString pluginName() const {return tr("Ethernet");}
    QIcon pluginIcon() const {return QIcon(":/EthernetConnectorPlugin/tabIcon");}
    QString pluginDescription() const {return tr("Connect to LRF sensor using ethernet interface");}
    PluginVersion pluginVersion() const {return PluginVersion(1, 0, 2);}
    QString pluginAuthorName() const {return "Mehrez Kristou";}
    QString pluginAuthorContact() const {return "mehrez@kristou.com";}
    int pluginLoadOrder() const {return 1;}
    bool pluginIsExperimental() const { return false; }

    QString pluginLicense() const { return "GPL"; }
    QString pluginLicenseDescription() const { return "GPL"; }

    void loadTranslator(const QString &locale);

    void saveState(QSettings &settings);
    void restoreState(QSettings &settings);

    void setupConnections();

    void setupShortcuts();

    bool isIntensityMode();

signals:
    void languageChanged();

public slots:
    void restart();
    void start();
    void stop();
    void pause();
    void errorHandle();
    void onLoad(PluginManagerInterface *manager);
    void onUnload();

protected:
    void changeEvent(QEvent* e);

private:
    void updateUiWithInfo();
    bool setCaptureMode();

    Ui::EthernetConnectorPlugin* ui;

    Thread *m_receiving_thread;
    UrgDevice *m_sensor;
    bool m_paused;
    bool m_connected;
    bool m_started;
    QTime m_stopWatch;
    QTranslator m_translator;


    clock_t m_startTimer;
    EthernetSettingsForm *m_settings;

    HelperPluginInterface *m_recorder;
    HelperPluginInterface *m_scipTerminal;
    HelperPluginInterface *m_ipChanger;

    EthernetConnectionWidget* m_connection_widget;
    RangeViewWidget* m_range_view_widget;
#ifdef USE_DEVICE_NET
    TcpDevice m_connectionDevice;
#else
    TcpipSocket connectionDevice;
#endif
    EthernetConnectionCheck m_connectionCheck;

    HelperPluginInterface *m_sensorInfo;

    void addDebugInformation(const QString &info);
    bool shouldStop();
    void addRecorder(PluginManagerInterface *manager);
    void addScipTerminal(PluginManagerInterface *manager);
    void addIpChanger(PluginManagerInterface *manager);
    void addWaveViewer(PluginManagerInterface *manager);
    void addSensorInformation(PluginManagerInterface *manager);

private slots:
    void connectPressed(bool connection, const QString &device, unsigned short port);
    void deviceConnected();
    void deviceConnectFailed();

    void startStepChanged(int value);
    void endStepChanged(int value);
    void groupStepsChanged(int value);
    void skipScansChanged(int value);

    void captureModeChanged();

    void receivingTimerProcess();
    void updateCaptureMode(RangeCaptureMode mode);
    void resetUi();
    void stepSpinChanged(int value);
    void ariaComboChanged(int value);
    void mirrorSpinhanged(int value);

    void initProgress(int value);

    void receivingThreadFinished();
    void errorHappened();

    void rebootSensor();
    void resetSensor();
};



#endif // ETHERNETCOMMUNICATORWIDGET_H

