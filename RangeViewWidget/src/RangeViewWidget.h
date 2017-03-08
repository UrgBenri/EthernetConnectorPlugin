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

#ifndef RANGE_VIEW_WIDGET_H
#define RANGE_VIEW_WIDGET_H

/*!
  \file
  \brief 範囲を 2D 表示するウィジット

  \author Satofumi KAMIMURA

  $Id: RangeViewWidget.h 55 2012-06-12 04:30:58Z kristou $
*/

#include <QWidget>

/*!
  \brief データ取得範囲の描画ウィジット
*/
class RangeViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RangeViewWidget(QWidget* parent);
    virtual ~RangeViewWidget(void);
    void setRange(int first_index_, int last_index_);
    void setParameters(int area_front_,  int area_total_);
    void unsetParameters();

protected:
    void paintEvent(QPaintEvent* event);

private:
    RangeViewWidget(const RangeViewWidget &rhs);
    RangeViewWidget &operator = (const RangeViewWidget &rhs);

    double index2rad(int index);
    QPoint calculatePoint(int radius, double radian);
    int area_front;
    int area_total;
    int first_index;
    int last_index;
};

#endif /* !RANGE_VIEW_WIDGET_H */

