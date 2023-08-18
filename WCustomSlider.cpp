#include "WCustomSlider.h"

WCustomSlider::WCustomSlider(QWidget *parent)
    : QSlider(parent)
{
    this->setMaximum(100);
}

WCustomSlider::~WCustomSlider()
{
}

void WCustomSlider::mousePressEvent(QMouseEvent *ev)
{
    //注意应先调用父类的鼠标点击处理事件，这样可以不影响拖动的情况
    QSlider::mousePressEvent(ev);
    //获取鼠标的位置，这里并不能直接从ev中取值（因为如果是拖动的话，鼠标开始点击的位置没有意义了）
    double pos = ev->pos().x() / (double)width();
    setValue(pos * (maximum() - minimum()) + minimum());

    emit sigCustomSliderValueChanged();
    mIsPressed = true;
}

void WCustomSlider::mouseReleaseEvent(QMouseEvent *ev)
{
    QSlider::mouseReleaseEvent(ev);

    //emit SigCustomSliderValueChanged();
    mIsPressed = false;
}

void WCustomSlider::mouseMoveEvent(QMouseEvent *ev)
{
    QSlider::mouseMoveEvent(ev);
    if (mIsPressed)
    {
        //获取鼠标的位置，这里并不能直接从ev中取值（因为如果是拖动的话，鼠标开始点击的位置没有意义了）
        double pos = ev->pos().x() / (double)width();
        setValue(pos * (maximum() - minimum()) + minimum());

        emit sigCustomSliderValueChanged();
    }
}
