﻿#pragma once

#include <QSlider>
#include <QMouseEvent>

class WCustomSlider : public QSlider
{
    Q_OBJECT

public:
	WCustomSlider(QWidget* parent);
    ~WCustomSlider();
protected:
    void mousePressEvent(QMouseEvent *ev);//重写QSlider的mousePressEvent事件
    void mouseReleaseEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
signals:
    void sigCustomSliderValueChanged(double pos);//自定义的鼠标单击信号，用于捕获并处理

private:
    bool mIsPressed = false;
};
