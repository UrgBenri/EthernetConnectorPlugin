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

#ifndef ETHERNETDATACONVERTER_H
#define ETHERNETDATACONVERTER_H

#include "PluginDataStructure.h"
#include "Converter.h"

class EthernetDataConverter : public DataConverterInterface
{
public:
    EthernetDataConverter(const Converter &conv);
    EthernetDataConverter(int frontStep = 0, int totalSteps = 0, int firstStep = 0, int lastStep = 0, int grouping = 0);
    virtual ~EthernetDataConverter();

    // DataConverterInterface interface
public:
    virtual QVector3D range2point(int step, long range, DataOffset offset, long maxLength) const;
    virtual QVector<QVector<QVector3D> > getPoints(const QVector<QVector<long> > &steps, DataOffset offset, int max_length) const;
    virtual int angle2index(qreal phy, qreal theta) const;
    virtual int index2Step(int index) const;
    virtual int step2Index(int step) const;
    virtual QPair<qreal, qreal> index2angle(int index) const;

private:
    int m_frontStep;
    int m_totalSteps;
    int m_firstStep;
    int m_lastStep;
    int m_grouping;

    qreal index2rad(int index) const;
};

#endif // ETHERNETDATACONVERTER_H

