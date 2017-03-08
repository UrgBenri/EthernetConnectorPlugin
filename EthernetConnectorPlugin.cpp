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

#include "EthernetConnectorPlugin.h"
#include "ui_EthernetConnectorPlugin.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QStandardPaths>
#include <QDesktopWidget>
#include <QEventLoop>
#include <QTimer>

#include "delay.h"

//#include "SettingsWindow.h"
#include "EthernetDataConverter.h"

#include "PluginUtils.h"

#include <QDebug>

const char* UrgDefaultAddress = "192.168.0.10";
const int UrgDefaultPort = 10940;
const QString LogHeader = "EthernetConnectorPlugin";

EthernetConnectorPlugin::EthernetConnectorPlugin(QWidget* parent)
    : ConnectorPluginInterface(parent)
    , ui(new Ui::EthernetConnectorPlugin)
    , m_receiving_thread(Q_NULLPTR)
    , m_sensor(Q_NULLPTR)
    , m_connection_widget(Q_NULLPTR)
    , m_range_view_widget(Q_NULLPTR)
    , m_settings(Q_NULLPTR)
    , m_recorder(Q_NULLPTR)
    , m_scipTerminal(Q_NULLPTR)
    , m_ipChanger(Q_NULLPTR)
    , m_sensorInfo(Q_NULLPTR)
{
    m_receiving_thread = new Thread(&receivingThreadProcess, this);
    ui->setupUi(this);
    m_connected = false;

    m_sensor = new UrgDevice();

    m_settings = new EthernetSettingsForm();
    m_settings->setWindowFlags(Qt::Popup);

    ui->loadActivityIndicator->hide();
    ui->moreButton->setEnabled(false);

    ui->rebootButton->setEnabled(false);
    ui->resetButton->setEnabled(false);
    ui->cmdButton->setEnabled(false);
    ui->cmdButton->setVisible(false);

    ui->skipScanWidget->setEnabled(false);

    m_connection_widget = ui->connectionWidget;

    m_sensor->setConnection(&m_connectionDevice);
    ui->recorder->setVisible(false);

    connect(ui->settingsButton, &QToolButton::clicked,
            this, [=](){
        QPoint position = ui->settingsButton->mapToGlobal(QPoint(ui->settingsButton->width(), ui->settingsButton->height()));
        if(position.y() + m_settings->geometry().height() > QApplication::desktop()->screenGeometry().height()){
            position.setY(position.y() - m_settings->geometry().height() - ui->settingsButton->height());
        }
        if(position.x() + m_settings->geometry().width() > QApplication::desktop()->screenGeometry().width()){
            position.setX(position.x() - m_settings->geometry().width() + ui->settingsButton->width());
        }
        m_settings->move(position);
        m_settings->show();
        m_settings->raise();
    });

    m_connection_widget->setPort(UrgDefaultPort);
    //    m_connection_widget->setPortVisible(false);

    ui->moreButton->setVisible(false);
    setupConnections();

    m_range_view_widget = ui->rangeViewWidget;

    m_paused = false;
    m_connected = false;
    m_started = false;

    resetUi();
}

void EthernetConnectorPlugin::addDebugInformation(const QString &info)
{
    emit information(LogHeader, info);
}

void EthernetConnectorPlugin::setupConnections()
{
    connect(m_connection_widget, &EthernetConnectionWidget::connectRequest,
            this, &EthernetConnectorPlugin::connectPressed);
    connect(&m_connectionCheck, &EthernetConnectionCheck::connected,
            this, &EthernetConnectorPlugin::deviceConnected);
    connect(&m_connectionCheck, &EthernetConnectionCheck::connectionFailed,
            this, &EthernetConnectorPlugin::deviceConnectFailed);

    connect(ui->captureMode, &ModeSwitcherWidget::captureModeChanged,
            this, &EthernetConnectorPlugin::captureModeChanged);

    connect(ui->startStep, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &EthernetConnectorPlugin::startStepChanged);
    connect(ui->endStep, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &EthernetConnectorPlugin::endStepChanged);
    connect(ui->groupSteps, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &EthernetConnectorPlugin::groupStepsChanged);
    connect(ui->skipScans, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &EthernetConnectorPlugin::skipScansChanged);


    connect(&m_connectionCheck, &EthernetConnectionCheck::progress,
            this, &EthernetConnectorPlugin::initProgress);

    connect(m_receiving_thread, &Thread::finished,
            this, &EthernetConnectorPlugin::receivingThreadFinished);


    connect(ui->rebootButton, &QAbstractButton::clicked,
            this, &EthernetConnectorPlugin::rebootSensor);

    connect(ui->resetButton, &QAbstractButton::clicked,
            this, &EthernetConnectorPlugin::resetSensor);

}

void EthernetConnectorPlugin::setupShortcuts()
{
}

bool EthernetConnectorPlugin::isIntensityMode()
{
    return ui->captureMode->isIntensitySelected();
}

EthernetConnectorPlugin::~EthernetConnectorPlugin()
{
    //    qDebug() << "EthernetCommunicatorWidget::~EthernetCommunicatorWidget";

    if(m_receiving_thread){
        if(m_receiving_thread->isRunning()){
            m_receiving_thread->stop();
        }
        delete m_receiving_thread;
    }
    if(m_sensor){
        m_sensor->stop();
        if(m_sensor->connection()) m_sensor->connection()->disconnect();

        m_connected = false;

        delete m_sensor;
    }

    if(m_settings) delete m_settings;
    if(m_scipTerminal) delete m_scipTerminal;
    if(m_ipChanger) delete m_ipChanger;

    delete ui;
}

void EthernetConnectorPlugin::start()
{
    qDebug() << "EthernetCommunicatorWidget::start";
    if (!m_connected) {
        return;
    }

    if (m_receiving_thread->isRunning()) {
        return;
    }

    addDebugInformation(tr("Acquisition started"));

    if (! m_paused) {
        if (!setCaptureMode()) {
            QMessageBox::critical(this, QApplication::applicationName(),
                                  tr("The connected device does not support the selected capture mode.") +
                                  "\n" +
                                  tr("Please choose another capture mode."));
            stop();
            return;
        }

        m_sensor->setCaptureRange(ui->startStep->value(), ui->endStep->value());
        m_sensor->setCaptureGroupSteps(ui->groupSteps->value());
        m_sensor->setCaptureFrameInterval(ui->skipScans->isVisible() ? ui->skipScans->value() : 0);


        ui->loadActivityIndicator->show();
    }
    else {
        m_paused = false;
        ui->loadActivityIndicator->show();
    }
    m_started = true;
    m_receiving_thread->run();
    emit started();
}

void EthernetConnectorPlugin::stop()
{
    qDebug() << "EthernetCommunicatorWidget::stop";
    addDebugInformation(tr("Stopping acquisition if active"));
    if (!m_connected) {
        return;
    }

    if(m_recorder) m_recorder->callFunction("stopMethod");

    if (m_receiving_thread->isRunning()) {
        m_receiving_thread->stop();
    }
    ui->loadActivityIndicator->hide();


    m_sensor->stop();
    addDebugInformation(tr("Sensor stopped"));

    m_paused = false;
    m_started = false;
    emit stopped();
}

void EthernetConnectorPlugin::pause()
{
    qDebug() << "EthernetCommunicatorWidget::pause";
    if (!m_connected) {
        return;
    }

    if (m_receiving_thread->isRunning()) {
        m_receiving_thread->stop();
    }
    ui->loadActivityIndicator->hide();

    m_paused = true;
    m_started = false;
    emit paused();
}

void EthernetConnectorPlugin::changeEvent(QEvent* e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            if(ui) ui->retranslateUi(this);
            emit languageChanged();
            break;
        default:
            break;
    }
}


void EthernetConnectorPlugin::updateCaptureMode(RangeCaptureMode mode)
{
    ui->captureMode->setCaptureMode(mode);
}

void EthernetConnectorPlugin::connectPressed(bool connection, const QString &device, unsigned short port)
{
    qDebug() << "EthernetCommunicatorWidget::connectPressed " << connection;

    if (connection) {
        addDebugInformation(tr("Connection requested"));
        m_connection_widget->setConnected(true);
        ui->loadActivityIndicator->setInvertedAppearance(true);
        ui->loadActivityIndicator->setValue(0);
        ui->loadActivityIndicator->show();
        m_connectionCheck.setConnectionRetriesCount(m_settings->connectionRetries());
        m_sensor->setConnectionDebug(m_settings->isDebugActive()
                                     , m_settings->debugDirectory()
                                     , m_settings->sendPrefix()
                                     , m_settings->receivePrefix());
        m_connectionCheck.setSensor(m_sensor);
        m_connectionCheck.setAddress(device);
        m_connectionCheck.setPort(port);
        m_connectionCheck.check();
        m_connected = true;
    }
    else {
        addDebugInformation(tr("Disconnection requested"));
        m_connectionCheck.stop();
        stop();
        resetUi();
        m_connected = false;

        if(m_recorder) m_recorder->setEnabled(m_connected);

        m_sensor->disconnect();
        addDebugInformation(tr("Sensor disconnected"));

        emit information(LogHeader,
                         tr("Ethernet device disconnected."));

        emit connexionLost();
    }

    ui->rebootButton->setEnabled(m_connected);
    ui->resetButton->setEnabled(m_connected);
    m_connection_widget->setConnected(m_connected);
    ui->moreButton->setEnabled(m_connected);
    ui->cmdButton->setEnabled(m_connected);
}


void EthernetConnectorPlugin::deviceConnected()
{
    bool errorPresent = false;
    addDebugInformation(tr("Device connected"));
    if (!m_sensor->loadParameters()) {
        emit error(QApplication::applicationName(),
                   m_sensor->what());
    }

    if(m_settings->isShowSensorErrorWarningActive()){
        QString sensorStatus = QString::fromStdString(m_sensor->internalInformation().sensorSituation);
        if(sensorStatus.toLower().contains("error") &&
                !sensorStatus.toLower().contains("no error")){
            QMessageBox::critical(this, QApplication::applicationName(),
                                  tr("The sensor is in error state.") + "\n" +
                                  sensorStatus);
            errorPresent = true;
        }
    }

    ui->loadActivityIndicator->hide();
    ui->loadActivityIndicator->setInvertedAppearance(false);
    updateUiWithInfo();
    m_connected = true;

    if(m_recorder) m_recorder->setEnabled(m_connected);

    emit information(LogHeader,
                     tr("Ethernet device connected."));

    if(errorPresent){
        if(m_sensorInfo) m_sensorInfo->callFunction("noReloadMethod", !m_started);
    }else{
        if (m_settings->isAutoStartActive()) {
            start();
        }
    }

    emit connexionReady();

    ui->rebootButton->setEnabled(m_connected);
    ui->resetButton->setEnabled(m_connected);
    m_connection_widget->setConnected(m_connected);
    ui->moreButton->setEnabled(m_connected);
    ui->cmdButton->setEnabled(m_connected);
}

void EthernetConnectorPlugin::deviceConnectFailed()
{
    qDebug() << "EthernetCommunicatorWidget::deviceConnectFailed";
    ui->loadActivityIndicator->hide();
    ui->loadActivityIndicator->setInvertedAppearance(false);
    stop();
    m_connected = false;
    resetUi();

    if(m_recorder) m_recorder->setEnabled(m_connected);

    QMessageBox::critical(this, QApplication::applicationName(),
                          tr("Connection failed.") + "\n" +
                          tr("Reason: %1").arg(m_sensor->what()));
    emit connexionLost();

    ui->rebootButton->setEnabled(m_connected);
    ui->resetButton->setEnabled(m_connected);
    m_connection_widget->setConnected(m_connected);
    ui->moreButton->setEnabled(m_connected);
    ui->cmdButton->setEnabled(m_connected);
}


void EthernetConnectorPlugin::updateUiWithInfo()
{
    qDebug() << "EthernetCommunicatorWidget::updateUiWithInfo";

    //    RangeSensorInformation urgInfo = m_urg->information();
    RangeSensorParameter urgParam = m_sensor->parameter();

    ui->modelLine->setText(m_sensor->getType());
    ui->serialLine->setText(m_sensor->getSerialNumber());

    ui->startStep->setMinimum(m_sensor->minArea());
    ui->startStep->setMaximum(m_sensor->maxArea());
    ui->startStep->setValue(m_sensor->minArea());


    ui->endStep->setMinimum(m_sensor->minArea());
    ui->endStep->setMaximum(m_sensor->maxArea());
    ui->endStep->setValue(m_sensor->maxArea());

    m_range_view_widget->setParameters(urgParam.area_front, urgParam.area_total);
    m_range_view_widget->setRange(m_sensor->minArea(), m_sensor->maxArea());

    if(m_settings->isCheckSupportedModesChecked()){
        ui->captureMode->setSupportedModes(m_sensor->supportedModes());
    }
}

void EthernetConnectorPlugin::resetUi()
{
    ui->modelLine->setText("");
    ui->serialLine->setText("");

    ui->startStep->setValue(0);
    ui->startStep->setMinimum(0);
    ui->startStep->setMaximum(0);
    ui->endStep->setValue(0);
    ui->endStep->setMinimum(0);
    ui->endStep->setMaximum(0);
    ui->groupSteps->setValue(1);

    //    ui->modesGroup->setEnabled(true);
    m_range_view_widget->unsetParameters();

    //    ui->captureMode->reset();
    ui->extraGroup->setVisible(ui->extraGroup->layout()->count() > 0);
}

void EthernetConnectorPlugin::stepSpinChanged(int value)
{
    qDebug() << "EthernetCommunicatorWidget::stepSpinChanged " << value;
    Q_UNUSED(value);
    restart();
}

void EthernetConnectorPlugin::ariaComboChanged(int value)
{
    qDebug() << "EthernetCommunicatorWidget::ariaComboChanged " << value;
    Q_UNUSED(value);
    restart();
}

void EthernetConnectorPlugin::mirrorSpinhanged(int value)
{
    qDebug() << "EthernetCommunicatorWidget::mirrorSpinhanged " << value;
    Q_UNUSED(value);
    restart();
}

void EthernetConnectorPlugin::initProgress(int value)
{
    ui->loadActivityIndicator->setValue(value);
}

bool EthernetConnectorPlugin::shouldStop()
{
    if (m_started) {
        switch (QMessageBox::question(
                    this,
                    QApplication::applicationName(),
                    tr("Data acquisition is running.") + "\n" +
                    tr("Do you want to stop it?"),
                    QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::No)
                ) {
            case QMessageBox::Yes:
                stop();
                break;
            case QMessageBox::No:
                return true;
                break;
            default:
                break;
        }
    }

    return false;
}

void EthernetConnectorPlugin::addRecorder(PluginManagerInterface *manager)
{
    if(manager){
        GeneralPluginInterface *recorderInterface = manager->makePlugin("org.kristou.UrgBenri.LogRecorderHelperPlugin", this);
        if(recorderInterface){
            m_recorder = qobject_cast<HelperPluginInterface *>(recorderInterface);
            if(m_recorder){
                if(m_recorder->hasFunction("setDeviceMethod")
                        && m_recorder->hasFunction("addSensorDataMethod")
                        && m_recorder->hasFunction("stopMethod")){
                    m_recorder->callFunction("setDeviceMethod", QVariantList() << qVariantFromValue((void *)m_sensor));
                    ui->recorder->layout()->addWidget(m_recorder);
                    m_recorder->setEnabled(false);
                    ui->recorder->setVisible(true);
                }
                else{
                    delete m_recorder;
                    m_recorder = Q_NULLPTR;
                }
            }
        }
    }
}
void EthernetConnectorPlugin::addScipTerminal(PluginManagerInterface *manager)
{
    if(manager){
        GeneralPluginInterface *pluginInterface = manager->makePlugin("org.kristou.UrgBenri.ScipTerminalHelperPlugin", Q_NULLPTR);
        if(pluginInterface){
            m_scipTerminal = qobject_cast<HelperPluginInterface *>(pluginInterface);
            if(m_scipTerminal){
                if(m_scipTerminal->hasFunction("setDeviceMethod")){
                    m_scipTerminal->callFunction("setDeviceMethod", QVariantList() << qVariantFromValue((void *)m_sensor));
                    connect(ui->cmdButton, &QPushButton::clicked,
                            this, [=](){
                        if (!m_sensor->isConnected())return;
                        if(m_scipTerminal) {
                            if(shouldStop()) return;
                            showPluginModal(m_scipTerminal, this);
                        }
                    });
                    ui->cmdButton->setIcon(pluginInterface->pluginIcon());
                    ui->cmdButton->setVisible(true);
                }
                else{
                    delete m_scipTerminal;
                    m_scipTerminal = Q_NULLPTR;
                }
            }
        }
    }
}

void EthernetConnectorPlugin::addIpChanger(PluginManagerInterface *manager)
{
    if(manager){
        GeneralPluginInterface *pluginInterface = manager->makePlugin("org.kristou.UrgBenri.IPChangerHelperPlugin", Q_NULLPTR);
        if(pluginInterface){
            m_ipChanger = qobject_cast<HelperPluginInterface *>(pluginInterface);
            if(m_ipChanger){
                if(m_ipChanger->hasFunction("setDeviceMethod")
                        && m_ipChanger->hasFunction("lastIPAddressMethod")
                        && m_ipChanger->hasFunction("setIPAddressMethod")){

                    m_ipChanger->callFunction("setDeviceMethod", qVariantFromValue((void *)m_sensor));

                    QPushButton *button = new QPushButton(tr("Change IP Address"), this);
                    button->setEnabled(false);
                    ui->extraGroup->layout()->addWidget(button);

                    emit information(LogHeader, QStringLiteral("IP Changer added"));

                    connect(this, &EthernetConnectorPlugin::languageChanged,
                            this, [=](){
                        if(button) button->setText(tr("Change IP Address"));
                    });
                    connect(button, &QAbstractButton::clicked,
                            this, [=](){
                        if (!m_sensor->isConnected()) {
                            emit information(LogHeader,
                                             tr("The sensor is not connected."));
                            return;
                        }
                        bool wasStarted = false;
                        if(m_started){
                            if(shouldStop()) return;
                            wasStarted = true;
                        }
                        m_ipChanger->callFunction("setIPAddressMethod"
                                                  , m_sensor->connection()->getDevice());

                        showPluginModal(m_ipChanger, this);

                        if(m_sensor->isConnected()){
                            if(wasStarted){
                                start();
                            }
                        }else{

                            m_connection_widget->setAddress(m_ipChanger->callFunction("lastIPAddressMethod").toString());

                            if(QMessageBox::information(this, QApplication::applicationName(),
                                                        tr("Would you like to reconnect to the sensor?"),
                                                        QMessageBox::Yes | QMessageBox::No,
                                                        QMessageBox::No) == QMessageBox::Yes
                                    ) {
                                connectPressed(false, NULL, 0);
                                ui->connectionWidget->connectSelectedDevice();
                            }
                            else{
                                connectPressed(false, NULL, 0);
                            }
                        }
                    });

                    connect(this, &EthernetConnectorPlugin::connexionReady,
                            this, [=](){
                        if(button) button->setEnabled(true);
                    });

                    connect(this, &EthernetConnectorPlugin::connexionLost,
                            this, [=](){
                        if(button) button->setEnabled(false);
                    });


                    button->setIcon(pluginInterface->pluginIcon());
                    button->setVisible(true);
                }
                else{
                    delete m_ipChanger;
                    m_ipChanger = Q_NULLPTR;
                }
            }
        }
    }
}

void EthernetConnectorPlugin::addWaveViewer(PluginManagerInterface *manager)
{
    if(manager){
        GeneralPluginInterface *pluginInterface = manager->makePlugin("org.kristou.UrgBenri.WaveViewerHelperPlugin", Q_NULLPTR);
        if(pluginInterface){
            HelperPluginInterface *waveViewer = qobject_cast<HelperPluginInterface *>(pluginInterface);
            if(waveViewer){
                if(waveViewer->hasFunction("setDeviceMethod")){
                    qCritical() << "Start";

                    waveViewer->callFunction("setDeviceMethod", qVariantFromValue((void *)m_sensor));

                    QPushButton *button = new QPushButton(tr("Wave Viewer"), this);
                    button->setEnabled(false);
                    ui->extraGroup->layout()->addWidget(button);

                    emit information(LogHeader, QStringLiteral("Wave Viewer added"));

                    connect(this, &EthernetConnectorPlugin::languageChanged,
                            this, [=](){
                        if(button) button->setText(tr("Wave Viewer"));
                    });
                    connect(button, &QAbstractButton::clicked,
                            this, [=](){
                        if (!m_sensor->isConnected()) {
                            emit information(LogHeader,
                                             tr("The sensor is not connected."));
                            return;
                        }
                        bool wasStarted = false;
                        if(m_started){
                            if(shouldStop()) return;
                            wasStarted = true;
                        }

                        showPluginModal(waveViewer, this);

                        if(m_sensor->isConnected()){
                            if(wasStarted){
                                start();
                            }
                        }
                    });

                    connect(this, &EthernetConnectorPlugin::connexionReady,
                            this, [=](){
                        if(button) button->setEnabled(true);
                    });

                    connect(this, &EthernetConnectorPlugin::connexionLost,
                            this, [=](){
                        if(button) button->setEnabled(false);
                    });


                    button->setIcon(pluginInterface->pluginIcon());
                    button->setVisible(true);
                }
                else{
                    delete waveViewer;
                }
            }
        }
    }
}

void EthernetConnectorPlugin::addSensorInformation(PluginManagerInterface *manager)
{
    if(manager){
        GeneralPluginInterface *pluginInterface = manager->makePlugin("org.kristou.UrgBenri.SensorInformationHelperPlugin", Q_NULLPTR);
        if(pluginInterface){
            m_sensorInfo = qobject_cast<HelperPluginInterface *>(pluginInterface);
            if(m_sensorInfo){
                if(m_sensorInfo->hasFunction("setDeviceMethod")
                        && m_sensorInfo->hasFunction("noReloadMethod")){
                    m_sensorInfo->callFunction("setDeviceMethod", qVariantFromValue((void *)m_sensor));
                    connect(ui->moreButton, &QPushButton::clicked,
                            this, [&](){
                        if (!m_sensor->isConnected()) {
                            emit information(LogHeader,
                                             tr("The sensor is not connected."));
                            return;
                        }
                        m_sensorInfo->callFunction("noReloadMethod", !m_started);
                        showPluginModal(m_sensorInfo, this);
                    });
                    ui->moreButton->setIcon(pluginInterface->pluginIcon());
                    ui->moreButton->setVisible(true);
                }
                else{
                    delete m_sensorInfo;
                    m_sensorInfo = Q_NULLPTR;
                }
            }
        }
    }
}

void EthernetConnectorPlugin::rebootSensor()
{
    qDebug() << "EthernetCommunicatorWidget::rebootSensor";

    if (m_sensor->isConnected()) {
        if(shouldStop()) return;
        QVector<string> lines;
        m_sensor->commandLines("RB\n", lines);
        m_sensor->commandLines("RB\n", lines);

        QEventLoop ev;
        QTimer timer;
        connect(&timer, &QTimer::timeout,
                &ev, &QEventLoop::quit);
        timer.start(1000);
        ev.exec();

        if(QMessageBox::information(this, QApplication::applicationName(),
                                    tr("Would you like to reconnect to the sensor?"),
                                    QMessageBox::Yes | QMessageBox::No,
                                    QMessageBox::No) == QMessageBox::Yes
                ) {
            connectPressed(false, NULL, 0);
            ui->connectionWidget->connectSelectedDevice();
        }
        else{
            connectPressed(false, NULL, 0);
        }

        emit information(LogHeader,
                         tr("The sensor is rebooted."));
    }
    else {
        emit information(LogHeader,
                         tr("The sensor is not connected."));
    }
}

void EthernetConnectorPlugin::resetSensor()
{
    qDebug() << "EthernetCommunicatorWidget::resetSensor";

    if (m_sensor->isConnected()) {
        bool started = isStarted();
        if(started && shouldStop()) return;
        QVector<string> lines;
        if(m_sensor->commandLines("RS\n", lines)){
            emit information(LogHeader,
                             tr("The sensor is reset."));
        }
        if(started) start();
    }
    else {
        emit information(LogHeader,
                         tr("The sensor is not connected."));
    }
}

void EthernetConnectorPlugin::receivingThreadFinished()
{
    qDebug() << "EthernetCommunicatorWidget::receivingThreadFinished";
    emit information(LogHeader,
                     tr("Data acquisition finished."));
    if(!m_receiving_thread->isRunning()) stop();
}

bool EthernetConnectorPlugin::setCaptureMode()
{
    RangeCaptureMode captureMode = ui->captureMode->captureMode();

    if (m_sensor->isSupportedMode(captureMode)) {
        m_sensor->setCaptureMode(captureMode);
        return true;
    }
    else {
        return false;
    }
}

void EthernetConnectorPlugin::startStepChanged(int value)
{
    qDebug() << "EthernetCommunicatorWidget::startStepChanged " << value;
    ui->endStep->setMinimum(value);

    m_range_view_widget->setRange(ui->startStep->value(), ui->endStep->value());
    restart();
}


void EthernetConnectorPlugin::endStepChanged(int value)
{
    qDebug() << "EthernetCommunicatorWidget::endStepChanged " << value;
    ui->startStep->setMaximum(value);

    m_range_view_widget->setRange(ui->startStep->value(), ui->endStep->value());
    restart();
}

void EthernetConnectorPlugin::groupStepsChanged(int value)
{
    qDebug() << "EthernetCommunicatorWidget::groupStepsChanged " << value;
    Q_UNUSED(value);
    restart();
}


void EthernetConnectorPlugin::skipScansChanged(int value)
{
    qDebug() << "EthernetCommunicatorWidget::skipScansChanged " << value;
    Q_UNUSED(value);
    restart();
}

void EthernetConnectorPlugin::captureModeChanged()
{
    ui->skipScanWidget->setEnabled(ui->captureMode->isContiousSelected());
    restart();
}

void EthernetConnectorPlugin::receivingTimerProcess()
{
    m_stopWatch.start();
    SensorDataArray l_ranges;
    SensorDataArray l_levels;
    long l_timeStamp = 0;

    int n = m_sensor->capture(l_ranges, l_levels, l_timeStamp);
    //qDebug() << "urg.capture";
    if (n <= 0) {
        return;
    }
    setLengthData(l_ranges, l_levels, l_timeStamp);

    qDebug() << "Elapsed time: " << m_stopWatch.elapsed() << "ms";
}

void  EthernetConnectorPlugin::restart()
{
    qDebug() << "EthernetCommunicatorWidget::restart";

    if (m_started) {
        stop();
        delay(m_sensor->scanMsec());
        start();
    }

    if (m_paused) {
        stop();
        delay(m_sensor->scanMsec());
        start();
        pause();
    }
}

int EthernetConnectorPlugin::receivingThreadProcess(void* args)
{
    qDebug() << "EthernetCommunicatorWidget::receivingThreadProcess";

    EthernetConnectorPlugin* obj = static_cast<EthernetConnectorPlugin*>(args);
    int errorCnt = 0;
    int maxErrorCnt = 5;

    qDebug() << "EthernetCommunicatorWidget thread started";

    SensorDataArray l_ranges;
    SensorDataArray l_levels;
    long l_timeStamp = 0;
    int n = 0;

    //    QTime timer;
    while (!obj->m_receiving_thread->exitThread) {
        if (errorCnt >= maxErrorCnt) {
            QMetaObject::invokeMethod(obj, "errorHandle");
            break;
        }

        //        timer.restart();
        n = obj->m_sensor->capture(l_ranges, l_levels, l_timeStamp);

        if (n == -2) { //Time out error
            errorCnt++;
            continue;
        }

        if(n <= 0){
            QMetaObject::invokeMethod(obj, "errorHappened");
        }

        obj->setLengthData(l_ranges, l_levels, l_timeStamp);

        errorCnt = 0;
    }

    qDebug() << "EthernetCommunicatorWidget thread finished";
    return 0;
}

void EthernetConnectorPlugin::errorHappened()
{
    qDebug() << "EthernetCommunicatorWidget::errorHappened";

    addDebugInformation(tr("Sensor error: %1").arg(m_sensor->what()));
}

void EthernetConnectorPlugin::errorHandle()
{
    qDebug() << "EthernetCommunicatorWidget::errorHandle";

    emit error(QApplication::applicationName(),
               tr("Error accured in the sensor.") + QString("\n")
               .arg(m_sensor->what()));
    addDebugInformation(tr("Error accured in the sensor.") + QString("\n"));

    if(m_settings->isAutoReconnectActive()){
        ui->connectionWidget->disconnectSelectedDevice();
        ui->connectionWidget->connectSelectedDevice();
    }
}

void EthernetConnectorPlugin::onLoad(PluginManagerInterface *manager)
{
    addRecorder(manager);
    addScipTerminal(manager);
    addIpChanger(manager);
    addSensorInformation(manager);
    addWaveViewer(manager);

    resetUi();
}

void EthernetConnectorPlugin::onUnload()
{

}

void EthernetConnectorPlugin::setLengthData(const SensorDataArray &l_ranges, const SensorDataArray &l_levels, long l_timeStamp)
{
    if(m_recorder) m_recorder->callFunction("addSensorDataMethod", QVariantList() << qVariantFromValue((void *)&l_ranges)
                                            << qVariantFromValue((void *)&l_levels)
                                            << qVariantFromValue((void *)&l_timeStamp));

    PluginDataStructure data;
    data.ranges = l_ranges.steps;
    data.levels = l_levels.steps;
    data.timestamp = l_timeStamp;
    data.converter = QSharedPointer<DataConverterInterface>(new EthernetDataConverter(l_ranges.converter));
    emit measurementDataReady(data);
}

void EthernetConnectorPlugin::loadTranslator(const QString &locale)
{
    qApp->removeTranslator(&m_translator);
    if (m_translator.load(QString("plugin.%1").arg(locale), ":/EthernetConnectorPlugin")) {
        qApp->installTranslator(&m_translator);
    }

    if(m_recorder) m_recorder->loadTranslator(locale);
    if(m_scipTerminal) m_scipTerminal->loadTranslator(locale);
    if(m_ipChanger) m_ipChanger->loadTranslator(locale);
    if(m_sensorInfo) m_sensorInfo->loadTranslator(locale);
}

void EthernetConnectorPlugin::saveState(QSettings &settings)
{
    settings.setValue("capture_mode", ui->captureMode->state());

    settings.setValue("address", m_connection_widget->address());
    settings.setValue("port", m_connection_widget->port());

    settings.setValue("start_step", ui->startStep->value());
    settings.setValue("end_step", ui->endStep->value());
    settings.setValue("skip_scan", ui->skipScans->value());
    settings.setValue("group_steps", ui->groupSteps->value());

    if(m_ipChanger){
        settings.beginGroup(m_ipChanger->metaObject()->className());
        m_ipChanger->saveState(settings);
        settings.endGroup();
    }
    if(m_scipTerminal){
        settings.beginGroup(m_scipTerminal->metaObject()->className());
        m_scipTerminal->saveState(settings);
        settings.endGroup();
    }
    if(m_recorder){
        settings.beginGroup(m_recorder->metaObject()->className());
        m_recorder->saveState(settings);
        settings.endGroup();
    }
    if(m_sensorInfo){
        settings.beginGroup(m_sensorInfo->metaObject()->className());
        m_sensorInfo->saveState(settings);
        settings.endGroup();
    }

    settings.beginGroup("SettingsDialog");
    m_settings->saveState(settings);
    settings.endGroup();
}

void EthernetConnectorPlugin::restoreState(QSettings &settings)
{
    ui->captureMode->setState(settings.value("capture_mode", "").toString());

    m_connection_widget->setAddress(settings.value("address", UrgDefaultAddress).toString());
    m_connection_widget->setPort(settings.value("port", UrgDefaultPort).toInt());

    ui->startStep->setValue(settings.value("start_step", ui->startStep->value()).toInt());
    ui->endStep->setValue(settings.value("end_step", ui->endStep->value()).toInt());
    ui->skipScans->setValue(settings.value("skip_scan", ui->skipScans->value()).toInt());
    ui->groupSteps->setValue(settings.value("group_steps", ui->groupSteps->value()).toInt());

    if(m_ipChanger){
        settings.beginGroup(m_ipChanger->metaObject()->className());
        m_ipChanger->restoreState(settings);
        settings.endGroup();
    }
    if(m_scipTerminal){
        settings.beginGroup(m_scipTerminal->metaObject()->className());
        m_scipTerminal->restoreState(settings);
        settings.endGroup();
    }
    if(m_recorder){
        settings.beginGroup(m_recorder->metaObject()->className());
        m_recorder->restoreState(settings);
        settings.endGroup();
    }
    if(m_sensorInfo){
        settings.beginGroup(m_sensorInfo->metaObject()->className());
        m_sensorInfo->restoreState(settings);
        settings.endGroup();
    }

    settings.beginGroup("SettingsDialog");
    m_settings->restoreState(settings);
    settings.endGroup();
}

