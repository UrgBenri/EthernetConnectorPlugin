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

#ifndef MODESWITCHBUTTON_H
#define MODESWITCHBUTTON_H

#include <QToolButton>

class ModeSwitchButton : public QToolButton
{
    Q_OBJECT
public:
    explicit ModeSwitchButton(QWidget *parent = 0);

    inline QStringList activeCommands() const { return m_activeCommands; }
    inline QStringList inactiveCommands() const { return m_inactiveCommands; }
    inline QStringList value() const { return m_supportedCommands; }

signals:
    void modeChanged();

public slots:
    void setActiveCommands(const QStringList &value);
    void setInactiveCommands(const QStringList &value);
    void setSupportedCommands(const QStringList &value);
    void setMode(const QString mode);
    void reset();

private:
    QStringList m_activeCommands;
    QStringList m_inactiveCommands;
    QStringList m_supportedCommands;
    bool m_isActive;
    bool m_isInactive;

    bool isSupported(const QStringList &modes, const QStringList &commands) const;
};

#endif // MODESWITCHBUTTON_H

