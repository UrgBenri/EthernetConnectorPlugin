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

#include "EthernetConnectionCheck.h"
#include <QSettings>
#include <QApplication>
#include <QDebug>
#include <QStandardPaths>
#include <QTime>

EthernetConnectionCheck::EthernetConnectionCheck()
{
    urg = NULL;
    address = "";
    port = 0;
    retries = 100;
    shouldStop = false;
}

EthernetConnectionCheck::~EthernetConnectionCheck()
{
}

void EthernetConnectionCheck::setConnectionRetriesCount(int count)
{
    retries = count;
}

void EthernetConnectionCheck::run()
{
    int retry = 0;
    while ((retry < retries) && !shouldStop) {
        int val = static_cast<int>(((double)(retry +1) / (double)retries) * 100.0);
        emit progress(val);
        if (urg) {
            if (urg->connect(address.toLatin1().constData(), port)) {
                emit progress(100);
                emit connected();
                return;
            }
        }

        QTime timer;
        timer.start();
        while(timer.elapsed() < 1000){
            if(shouldStop) break;
        }
        retry++;
    }

    emit progress(100);
    emit connectionFailed();

}

void EthernetConnectionCheck::stop()
{
    shouldStop = true;
    if(isRunning()){
        wait();
    }
}

void EthernetConnectionCheck::check()
{
    shouldStop = false;
    if(!isRunning()){
        start();
    }
}

