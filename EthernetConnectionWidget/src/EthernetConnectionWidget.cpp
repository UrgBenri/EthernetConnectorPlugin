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

#include "EthernetConnectionWidget.h"
#include <QFile>
#include <QTextStream>
#include <QCompleter>
#include <QSettings>
#include <QStandardPaths>
#include <QDebug>

using namespace std;


namespace
{
const char CompleterFile[] = "ethernet_address.txt";
const char DefauktIpAddress[] = "192.168.0.10";
}


struct EthernetConnectionWidget::pImpl {
    EthernetConnectionWidget* widget_;
    QStringList completer_address_;


    pImpl(EthernetConnectionWidget* widget) : widget_(widget) {
    }


    void initializeForm(void) {
        connect(widget_->connect_button_, &QAbstractButton::clicked,
                widget_, &EthernetConnectionWidget::connectPressed);
        connect(widget_->disconnect_button_, &QAbstractButton::clicked,
                widget_, &EthernetConnectionWidget::disconnectPressed);

        widget_->port_spinbox_->setRange(0, 65535);

        loadCompleteAddress();

        widget_->loadCurrentIPAddress();
    }


    bool loadCompleteAddress(void) {
        QFile file(CompleterFile);
        if (! file.open(QFile::ReadOnly)) {
            return false;
        }

        QTextStream fin(&file);
        forever {
            QString line = fin.readLine();
            if (line.isNull()) {
                break;
            }
            completer_address_ << line;
        }

        QCompleter* completer = new QCompleter(completer_address_, widget_);
        widget_->address_lineedit_->setCompleter(completer);

        return true;
    }


    bool saveCompleteAddress(void) {
        QFile file(CompleterFile);
        if (! file.open(QFile::WriteOnly | QFile::Truncate)) {
            return false;
        }

        QTextStream fout(&file);
        size_t n = completer_address_.size();
        for (size_t i = 0; i < n; ++i) {
            fout << completer_address_.at(i);
        }

        return true;
    }

    void setPortVisible(bool state) {
        if (state) {
            widget_->port_spinbox_->show();
        }
        else {
            widget_->port_spinbox_->hide();
        }
    }
};

void EthernetConnectionWidget::setPortVisible(bool state)
{
    pimpl->setPortVisible(state);
}


EthernetConnectionWidget::EthernetConnectionWidget(QWidget* parent)
    : QWidget(parent), pimpl(new pImpl(this))
{
    setupUi(this);
    pimpl->initializeForm();
//    address_lineedit_->setInputMask("090.090.090.090; ");

}

EthernetConnectionWidget::~EthernetConnectionWidget(void)
{
}

void EthernetConnectionWidget::saveCurrentIPAddress()
{
    QSettings settings(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/Settings.ini", QSettings::IniFormat);
    settings.setValue("EthernetConnectionWidget/LastSelectedIPAdress", address());
}

void EthernetConnectionWidget::loadCurrentIPAddress()
{
    QSettings settings(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/Settings.ini", QSettings::IniFormat);
    setAddress(settings.value("EthernetConnectionWidget/LastSelectedIPAdress", DefauktIpAddress).toString());
}


void EthernetConnectionWidget::setConnected(bool connected)
{
    if (connected) {
        QString connected_address = address();
        if (! pimpl->completer_address_.contains(connected_address)) {
            pimpl->completer_address_ << connected_address;
            pimpl->saveCompleteAddress();
            saveCurrentIPAddress();
        }

        connect_button_->setEnabled(false);
        disconnect_button_->setEnabled(true);
    }
    else {
        connect_button_->setEnabled(true);
        disconnect_button_->setEnabled(false);
    }
    address_lineedit_->setEnabled(! connected);
    port_spinbox_->setEnabled(! connected);
}


bool EthernetConnectionWidget::isConnected(void) const
{
    return !connect_button_->isEnabled();
}


void EthernetConnectionWidget::setAddress(const QString &address)
{
    address_lineedit_->setText(address);
}


QString EthernetConnectionWidget::address(void) const
{
    return address_lineedit_->text();
}


void EthernetConnectionWidget::setPort(unsigned short port)
{
    port_spinbox_->setValue(port);
}


unsigned EthernetConnectionWidget::port(void) const
{
    return port_spinbox_->value();
}


void EthernetConnectionWidget::setEnabled(bool enable)
{
    QWidget::setEnabled(enable);
}


void EthernetConnectionWidget::setFocus(void)
{
    connect_button_->setFocus();
}


void EthernetConnectionWidget::connectPressed()
{
    connect_button_->setEnabled(false);
    disconnect_button_->setEnabled(false);
    QString address = address_lineedit_->text();
    unsigned short port = port_spinbox_->value();
    emit connectRequest(true, address, port);
}

void EthernetConnectionWidget::disconnectPressed()
{
    connect_button_->setEnabled(false);
    disconnect_button_->setEnabled(false);
    QString address = address_lineedit_->text();
    unsigned short port = port_spinbox_->value();
    emit connectRequest(false, address, port);
}



void EthernetConnectionWidget::connectSelectedDevice()
{
    if(connect_button_->isEnabled()){
        connectPressed();
    }
}

void EthernetConnectionWidget::disconnectSelectedDevice()
{
    if(disconnect_button_->isEnabled()){
        disconnectPressed();
    }
}

void EthernetConnectionWidget::fireConnect()
{
    if(connect_button_->isEnabled()){
        connectPressed();
    }
}

void EthernetConnectionWidget::fireDisconnect()
{
    if(disconnect_button_->isEnabled()){
        disconnectPressed();
    }
}

