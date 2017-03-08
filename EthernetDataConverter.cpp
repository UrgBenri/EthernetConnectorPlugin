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

#include "EthernetDataConverter.h"
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

EthernetDataConverter::EthernetDataConverter(const Converter &conv)
    : DataConverterInterface()
{
    m_frontStep = conv.frontStep();
    m_totalSteps = conv.totalSteps();
    m_firstStep = conv.firstStep();
    m_lastStep = conv.lastStep();
    m_grouping = conv.grouping();
}

EthernetDataConverter::EthernetDataConverter(int frontStep
                                             , int totalSteps
                                             , int firstStep
                                             , int lastStep
                                             , int grouping)
    : m_frontStep(frontStep)
    , m_totalSteps(totalSteps)
    , m_firstStep(firstStep)
    , m_lastStep(lastStep)
    , m_grouping(grouping > 0 ? grouping : 1)
{

}

EthernetDataConverter::~EthernetDataConverter()
{

}

QVector3D EthernetDataConverter::range2point(int step
                                             , long range
                                             , DataOffset offset
                                             , long maxLength) const
{
    long distance = range > 0 ? range : 0;
    if(maxLength > 0)
    {
        if(distance > maxLength)
        {
            distance = maxLength;
        }
    }

    distance += offset.range;

    qreal angle = index2rad(step) + offset.phy;
    qreal x = static_cast<qreal>(distance * cos(angle));
    qreal y = static_cast<qreal>(distance * sin(angle));

    return QVector3D(x, y, 0);
}

QVector<QVector<QVector3D> > EthernetDataConverter::getPoints(const QVector<QVector<long> > &steps
                                                              , DataOffset offset
                                                              , int max_length) const
{
    QVector<QVector<QVector3D> > points;

    for (int i = 0; i < steps.size(); ++i) {
        QVector<QVector3D> point;
        for (int j = 0; j < steps[i].size(); ++j) {
            point << range2point(index2Step(i)
                                 , steps[i][j]
                                 , offset
                                 , max_length);
        }
        points << point;
    }

    return points;
}

int EthernetDataConverter::angle2index(qreal phy, qreal theta) const
{
    qreal resolution = m_totalSteps / (2.0 * M_PI);
    int step = qRound((resolution * phy) + 0.5);
    return qBound(m_firstStep, step + m_frontStep, m_lastStep);
}

int EthernetDataConverter::index2Step(int index) const
{
    return m_firstStep + (index * m_grouping);
}

int EthernetDataConverter::step2Index(int step) const
{
    return (step - m_firstStep) / m_grouping;
}

QPair<qreal, qreal> EthernetDataConverter::index2angle(int index) const
{
    return QPair<qreal, qreal>(index2rad(index) , 0);
}

qreal EthernetDataConverter::index2rad(int index) const
{
    index = qBound(m_firstStep, index, m_lastStep);
    int index_from_front = index - m_frontStep;
    return index_from_front * (2.0 * M_PI) / m_totalSteps;
}

