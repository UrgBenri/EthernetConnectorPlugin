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

#include "EthernetSettingsForm.h"
#include "ui_EthernetSettingsForm.h"

#include <QFileDialog>

EthernetSettingsForm::EthernetSettingsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EthernetSettingsForm)
{
    ui->setupUi(this);
    connect(ui->browseButton, &QToolButton::clicked,
            this, &EthernetSettingsForm::browseButtonClicked);

    checkDebugInformation();
}

EthernetSettingsForm::~EthernetSettingsForm()
{
    delete ui;
}

int EthernetSettingsForm::connectionRetries()
{
    return ui->connectionRetries->value();
}

int EthernetSettingsForm::isDebugActive()
{
    return ui->debugActiveCheck->isChecked() && !ui->debugFolder->text().isEmpty();
}

QString EthernetSettingsForm::debugDirectory()
{
    return ui->debugFolder->text();
}

QString EthernetSettingsForm::sendPrefix()
{
    return ui->sendPrefix->text();
}

QString EthernetSettingsForm::receivePrefix()
{
    return ui->receivePrefix->text();
}

bool EthernetSettingsForm::isAutoStartActive()
{
    return ui->scanAutostart->isChecked();
}

bool EthernetSettingsForm::isShowSensorErrorWarningActive()
{
    return ui->errorWarning->isChecked();
}

bool EthernetSettingsForm::isCheckSupportedModesChecked()
{
    return ui->checkSupportedModes->isChecked();
}

bool EthernetSettingsForm::isAutoReconnectActive()
{
    return ui->autoReconnect->isChecked();
}

void EthernetSettingsForm::saveState(QSettings &settings)
{
    settings.setValue("connectionRetries", ui->connectionRetries->value());
    settings.setValue("debugActiveCheck", ui->debugActiveCheck->isChecked());
    settings.setValue("debugFolder", ui->debugFolder->text());
    settings.setValue("sendPrefix", ui->sendPrefix->text());
    settings.setValue("receivePrefix", ui->receivePrefix->text());
    settings.setValue("scanAutostart", ui->scanAutostart->isChecked());
    settings.setValue("errorWarning", ui->errorWarning->isChecked());
    settings.setValue("checkSupportedModes", ui->checkSupportedModes->isChecked());
    settings.setValue("autoReconnect", ui->autoReconnect->isChecked());
}

void EthernetSettingsForm::restoreState(QSettings &settings)
{
    ui->connectionRetries->setValue(settings.value("connectionRetries", 100).toInt());
    ui->debugActiveCheck->setChecked(settings.value("debugActiveCheck", false).toBool());
    ui->debugFolder->setText(settings.value("debugFolder").toString());
    ui->sendPrefix->setText(settings.value("sendPrefix").toString());
    ui->receivePrefix->setText(settings.value("receivePrefix").toString());
    ui->scanAutostart->setChecked(settings.value("scanAutostart", true).toBool());
    ui->errorWarning->setChecked(settings.value("errorWarning", true).toBool());
    ui->checkSupportedModes->setChecked(settings.value("checkSupportedModes", true).toBool());
    ui->autoReconnect->setChecked(settings.value("autoReconnect", true).toBool());

    checkDebugInformation();
}

void EthernetSettingsForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            if(ui) ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void EthernetSettingsForm::browseButtonClicked()
{
    QString foldername = QFileDialog::getExistingDirectory(this, tr("Choose a directory"),
                                                           QDir::currentPath(),
                                                           QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!foldername.isEmpty()){
        ui->debugFolder->setText(foldername);
    }
}

void EthernetSettingsForm::checkDebugInformation()
{
    if(ui->debugFolder->text().isEmpty()){
        ui->debugFolder->setText(QApplication::applicationDirPath());
    }

    if(ui->sendPrefix->text().isEmpty()){
        ui->sendPrefix->setText("Sent_");
    }

    if(ui->receivePrefix->text().isEmpty()){
        ui->receivePrefix->setText("Recv_");
    }
}
