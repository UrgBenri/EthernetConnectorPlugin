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

#include "ModeSwitcherWidget.h"
#include "ui_ModeSwitcherWidget.h"

#include <QDebug>

ModeSwitcherWidget::ModeSwitcherWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModeSwitcherWidget)
{
    ui->setupUi(this);

    ui->continuousCheck->setActiveCommands(QStringList() << "MD" << "ME" << "ND" << "NE");
    ui->continuousCheck->setInactiveCommands(QStringList() << "GD" << "GE" << "HD" << "HE");

    ui->intensityCheck->setActiveCommands(QStringList() << "GE" << "ME" << "HE" << "NE");
    ui->intensityCheck->setInactiveCommands(QStringList() << "GD" << "MD" << "HD" << "ND");

    ui->multiechoCheck->setActiveCommands(QStringList() << "HD" << "HE" << "ND" << "NE");
    ui->multiechoCheck->setInactiveCommands(QStringList() << "GD" << "GE" << "MD" << "ME");

    m_modeButtons
            << ui->continuousCheck
            << ui->intensityCheck
            << ui->multiechoCheck;

    foreach (ModeSwitchButton *button, m_modeButtons) {
        button->setStyleSheet("text-align: left");
        connect(button, &ModeSwitchButton::modeChanged,
                this, &ModeSwitcherWidget::captureModeChanged);
    }

}

ModeSwitcherWidget::~ModeSwitcherWidget()
{
    delete ui;
}

void ModeSwitcherWidget::setCaptureMode(RangeCaptureMode mode)
{
    foreach (ModeSwitchButton *button, m_modeButtons) {
        button->setMode(captureMode2String(mode));
    }
}

QString ModeSwitcherWidget::captureMode2String(RangeCaptureMode mode)const
{

    switch (mode) {
        case GD_Capture_mode: return "GD";
        case GE_Capture_mode: return "GE";
        case HD_Capture_mode: return "HD";
        case HE_Capture_mode: return "HE";
        case MD_Capture_mode: return "MD";
        case ME_Capture_mode: return "ME";
        case ND_Capture_mode: return "ND";
        case NE_Capture_mode: return "NE";
    }
    return "--";
}

RangeCaptureMode ModeSwitcherWidget::captureMode()
{
    RangeCaptureMode captureMode = Unknown_Capture_mode;

    if (ui->intensityCheck->isChecked()) {
        if (ui->multiechoCheck->isChecked()) {
            if (ui->continuousCheck->isChecked()) {
                captureMode = NE_Capture_mode;
            }
            else {
                captureMode = HE_Capture_mode;
            }
        }
        else {
            if (ui->continuousCheck->isChecked()) {
                captureMode = ME_Capture_mode;
            }
            else {
                captureMode = GE_Capture_mode;
            }
        }
    }
    else {
        if (ui->multiechoCheck->isChecked()) {
            if (ui->continuousCheck->isChecked()) {
                captureMode = ND_Capture_mode;
            }
            else {
                captureMode = HD_Capture_mode;
            }
        }
        else {
            if (ui->continuousCheck->isChecked()) {
                captureMode = MD_Capture_mode;
            }
            else {
                captureMode = GD_Capture_mode;
            }
        }
    }

    return captureMode;
}

void ModeSwitcherWidget::reset()
{
    foreach (ModeSwitchButton *button, m_modeButtons) {
        button->reset();
    }
}

void ModeSwitcherWidget::setState(const QString &value)
{
    QStringList list = value.split(",", QString::SkipEmptyParts);
    if(list.size() == 3){
        ui->continuousCheck->setChecked(list[0].toLower().trimmed() == "true");
        ui->intensityCheck->setChecked(list[1].toLower().trimmed() == "true");
        ui->multiechoCheck->setChecked(list[2].toLower().trimmed() == "true");
    }
}

QString ModeSwitcherWidget::state()
{
    QStringList result;

    result << QString(ui->continuousCheck->isChecked() ? "true" : "false");
    result << QString(ui->intensityCheck->isChecked() ? "true" : "false");
    result << QString(ui->multiechoCheck->isChecked() ? "true" : "false");

    return result.join(",");
}

void ModeSwitcherWidget::setSupportedModes(const QStringList &modes)
{
    foreach (ModeSwitchButton *button, m_modeButtons) {
        button->setSupportedCommands(modes);
    }
}

bool ModeSwitcherWidget::isIntensitySelected()
{
    return ui->intensityCheck->isChecked();
}

bool ModeSwitcherWidget::isContiousSelected()
{
    return ui->continuousCheck->isChecked();
}

bool ModeSwitcherWidget::isMultiechoSelected()
{
    return ui->multiechoCheck->isChecked();
}

void ModeSwitcherWidget::changeEvent(QEvent *e)
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

