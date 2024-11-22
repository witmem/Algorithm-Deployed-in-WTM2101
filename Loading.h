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

#ifndef LOADING_H
#define LOADING_H

#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QMap>

struct Location
{
public:
    explicit Location(float _x,float _y){x=_x;y=_y;}
    float x;
    float y;
};

class Loading : public QWidget
{
    Q_OBJECT
public:
    explicit Loading(QWidget *parent = nullptr);
    //设置圆点个数
    void setDotCount(int);
    //设置点颜色
    void setDotColor(const QColor&);
    //开始
    void start();
    //设置圆点最大直径
    void setMaxDiameter(float);
    //设置圆点最小直径
    void setMinDiameter(float);

private:
    //刷新计数
    int _index;
    //点的颜色
    QColor _dotColor;
    //点的个数
    int _count;
    //圆点最小直径
    float _minDiameter;
    //圆点最大直径
    float _maxDiameter;
    //绘制圆点
    void paintDot(QPainter &);
    //计数
    int _i;
    //时间间隔 单位：毫秒(ms)
    int _interval;
    //定时器
    QTimer timer;
    //绘制区域边长
    float _squareWidth;
    //圆的直径
    float _centerDistance;
    //直径列表
    QList<float> radiiList;
    //圆点坐标列表
    QList<Location> locationList;

    //计算
    void caculate();

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
signals:

public slots:

private slots:
    void refresh();
};

#endif // LOADING_H
