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

#include "ModeSwitchButton.h"

#include <QDebug>

ModeSwitchButton::ModeSwitchButton(QWidget *parent)
    : QToolButton(parent)
    , m_isActive(true)
    , m_isInactive(true)
{
    setCheckable(true);

    connect(this, &ModeSwitchButton::toggled,
            this, [&](bool state){
        if((state && m_isActive) || (!state && m_isInactive)){
            emit modeChanged();
        }
        else{
            setChecked(!state);
        }
    });
}

void ModeSwitchButton::setActiveCommands(const QStringList &value)
{
    m_activeCommands = value;
}

void ModeSwitchButton::setInactiveCommands(const QStringList &value)
{
    m_inactiveCommands = value;
}

void ModeSwitchButton::setSupportedCommands(const QStringList &value)
{
    if(m_supportedCommands != value){
        m_supportedCommands = value;
        if(m_supportedCommands.isEmpty()){
            reset();
        }
        else{
            m_isActive = isSupported(m_supportedCommands, m_activeCommands);
            m_isInactive = isSupported(m_supportedCommands, m_inactiveCommands);
            bool active = m_isActive && m_isInactive;
            if(!active){
                setChecked(m_isActive);
            }
            setEnabled(active);
        }
    }
}

void ModeSwitchButton::setMode(const QString mode)
{
    if(m_activeCommands.contains(mode)){
        setChecked(true);
    }
    else if(m_inactiveCommands.contains(mode)){
        setChecked(false);
    }
}

void ModeSwitchButton::reset()
{
    m_supportedCommands.clear();
    m_isActive = true;
    m_isInactive = true;
    setChecked(false);
    setEnabled(true);
}

bool ModeSwitchButton::isSupported(const QStringList &modes, const QStringList &commands) const
{
    foreach (const QString &mode, modes) {
        if(commands.contains(mode)){
            return true;
        }
    }
    return false;
}

