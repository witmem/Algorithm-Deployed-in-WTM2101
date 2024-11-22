/******************************************************************************************
**  Copyright 2023 Hangzhou Zhicun (Witmem) Technology Co., Ltd.  All rights reserved.   **
**                                                                                       **
**  This program is free software: you can redistribute it and/or modify                 **
**  it under the terms of the GNU General Public License as published by                 **
**  the Free Software Foundation, either version 3 of the License, or                    **
**  (at your option) any later version.                                                  **
**                                                                                       **
**  This program is distributed in the hope that it will be useful,                      **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of                       **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                        **
**  GNU General Public License for more details.                                         **
**                                                                                       **
**  You should have received a copy of the GNU General Public License                    **
**  along with this program.  If not, see http://www.gnu.org/licenses/.                  **
*******************************************************************************************/

#include "Loading.h"
#include "qmath.h"
#include <QDebug>

Loading::Loading(QWidget *parent) : QWidget(parent)
  ,_index(0)
  ,_i(0)
  ,_interval(50)

{
    setDotColor(QColor(55, 144, 250));
    setDotCount(15);
    connect(&timer,&QTimer::timeout,this,&Loading::refresh);
    setMaxDiameter(10);
    setMinDiameter(5);
}

//设置点的个数
void Loading::setDotCount(int count)
{
    _count=count;
}

//设置点的颜色
void Loading::setDotColor(const QColor & color)
{
    _dotColor=color;
}

//开始动画
void Loading::start()
{
    timer.setInterval(_interval);
    timer.start();
}

//设置最大直径
void Loading::setMaxDiameter(float max)
{
    _maxDiameter=max;
}

//设置最小直径
void Loading::setMinDiameter(float min)
{
    _minDiameter=min;
}

//刷新界面
void Loading::refresh()
{
    repaint();
}

void Loading::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    caculate();
}

void Loading::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(_dotColor);
    painter.setBrush(_dotColor);

    //绘制点
    paintDot(painter);
}

//计算绘制正方形区域
void Loading::caculate()
{
    _squareWidth=qMin(this->width(),this->height());
    float half=_squareWidth/2;
    _centerDistance=half-_maxDiameter/2-1;

    float gap=(_maxDiameter-_minDiameter)/(_count-1)/2;
    float angleGap=(float)360/_count;

    locationList.clear();
    radiiList.clear();

    for(int i=0;i<_count;i++)
    {
        radiiList<<_maxDiameter/2-i*gap;
        float radian=qDegreesToRadians(-angleGap*i);
        locationList.append(Location(half+_centerDistance*qCos(radian),half-_centerDistance*qSin(radian)));
    }
}

//绘制圆点
void Loading::paintDot(QPainter& painter)
{
    for(int i=0;i<_count;i++) {
        painter.setPen(_dotColor);
        //半径
        float radii=radiiList.at((_index+_count-i)%_count);

        //绘制圆点
        painter.drawEllipse(QPointF(locationList.at(i).x,locationList.at(i).y),radii,radii);
    }
    _index++;
}
