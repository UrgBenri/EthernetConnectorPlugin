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

#include "RangeViewWidget.h"
#include <QPainter>
#include <qmath.h>
#include <QtDebug>

using namespace std;

RangeViewWidget::RangeViewWidget(QWidget* parent)
    : QWidget(parent),
      area_front(0), area_total(0),
      first_index(0), last_index(0)
{
    setMinimumSize(100, 100);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}


RangeViewWidget::~RangeViewWidget(void)
{
}


void RangeViewWidget::paintEvent(QPaintEvent* event)
{
    static_cast<void>(event);

    QPainter painter;
    painter.begin(this);
    QRect this_rect = rect();
    painter.fillRect(this_rect, QBrush(QColor(Qt::white)));
    painter.drawRect(0, 0, this_rect.width() - 1, this_rect.height() - 1);

    enum { FontSize = 12 };
    QFont font("times", FontSize);
    QFontMetrics fm(font);
    size_t text_height = fm.height();
    size_t text_width = fm.width("-XXX°");

    const int offset = 2;
    int width_radius = ((this_rect.width() - text_width) / 2)  - offset;
    int height_radius = (this_rect.height() / 2) - text_height - offset;

    // 円の描画
    painter.setPen(QColor(Qt::gray));
    size_t radius = qMin(width_radius, height_radius);
    QPoint center(this_rect.width() / 2, this_rect.height() / 2);
    int cx = center.x() - radius;
    int cy = center.y() - radius;
    painter.drawEllipse(cx, cy,
                        static_cast<int>(radius * 2.02),
                        static_cast<int>(radius * 2.02));

    // X 軸, Y 軸の描画
    painter.drawLine(offset, center.y(), this_rect.width() - offset, center.y());
    painter.drawLine(center.x(), offset, center.x(), this_rect.height() - offset);

    painter.setPen(Qt::black);
    double first_radian = index2rad(first_index) + (M_PI / 2.0);
    double last_radian = index2rad(last_index) + (M_PI / 2.0);

    QPoint first_point =
        calculatePoint(radius + (offset * 2), first_radian);
    QPoint last_point =
        calculatePoint(radius + (text_height / 2) + offset, last_radian);
    first_point.setY(-first_point.y());
    first_point += center;
    last_point.setY(-last_point.y());
    last_point += center;

    int first_deg = qRound(first_radian * 180.0 / M_PI) - 90;
    int last_deg = qRound(last_radian * 180.0 / M_PI) - 90;

    if ((first_deg == 0) && (last_deg == 0)) {
        painter.setPen(QColor(Qt::black));
        painter.drawText(0, 0, this_rect.width(), this_rect.height() / 2,
                         Qt::AlignVCenter | Qt::AlignHCenter,
                         tr("no step parameters."));
        return;
    }

    painter.setBrush(QColor(0x33, 0x77, 0xff));
    painter.drawPie(cx, cy, radius * 2, radius * 2,
                    (first_deg + 90) * 16, (last_deg - first_deg) * 16);

    painter.drawText(first_point.x() + (offset * 3),
                     first_point.y() - (text_height / 2),
                     text_width, text_height,
                     Qt::AlignLeft | Qt::AlignBottom,
                     QString("%1°").arg(first_deg));

    painter.drawText(last_point.x() - text_width,
                     last_point.y() - (text_height / 2),
                     text_width, text_height,
                     Qt::AlignRight | Qt::AlignBottom,
                     QString("%1°").arg(last_deg));

    painter.end();
}

void RangeViewWidget::setRange(int first_index_, int last_index_)
{
    first_index = first_index_;
    last_index = last_index_;

    repaint();
}

void RangeViewWidget::setParameters(int area_front_, int area_total_)
{
    area_front = area_front_;
    area_total = area_total_;
    repaint();
}

void RangeViewWidget::unsetParameters()
{
    area_front = 0;
    area_total = 0;
    repaint();
}

double RangeViewWidget::index2rad(int index)
{
    if (area_total > 0) {
        int index_from_front = index - area_front;
        return index_from_front * (2.0 * M_PI) / area_total;
    }
    else {
        return 0;
    }
}


QPoint RangeViewWidget::calculatePoint(int radius, double radian)
{
    int x = static_cast<int>(radius * cos(radian));
    int y = static_cast<int>(radius * sin(radian));

    return QPoint(x, y);
}

