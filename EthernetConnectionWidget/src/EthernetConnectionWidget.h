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

#ifndef ETHERNET_CONNECTION_WIDGET_H
#define ETHERNET_CONNECTION_WIDGET_H


#include "ui_EthernetConnectionWidgetForm.h"
#include <string>
#include <memory>

class EthernetConnectionWidget
    : public QWidget, private Ui::EthernetConnectionWidgetForm
{
    Q_OBJECT

public:
    EthernetConnectionWidget(QWidget* parent = 0);
    virtual ~EthernetConnectionWidget(void);

    void setConnected(bool connected);
    bool isConnected(void) const;

    void setAddress(const QString &address);
    QString address(void) const;
    void setPort(unsigned short port);
    unsigned port(void) const;

    void setEnabled(bool enable);
    void setFocus(void);

    void setPortVisible(bool state);

    void connectSelectedDevice();
    void disconnectSelectedDevice();


    void fireConnect();
    void fireDisconnect();

signals:
    void connectRequest(bool connection,
                        const QString &address, unsigned short port);

private slots:
    void connectPressed();
    void disconnectPressed();

private:
    EthernetConnectionWidget(const EthernetConnectionWidget &rhs);
    EthernetConnectionWidget &operator = (const EthernetConnectionWidget &rhs);

    struct pImpl;
    std::auto_ptr<pImpl> pimpl;
    void saveCurrentIPAddress();
    void loadCurrentIPAddress();
};

#endif /* !ETHERNET_CONNECTION_WIDGET_H */

