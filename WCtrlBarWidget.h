#pragma once

#include <QWidget>
#include "ui_WCtrlBarWidget.h"

class WCtrlBarWidget : public QWidget
{
	Q_OBJECT

public:
	WCtrlBarWidget(QWidget *parent = nullptr);
	~WCtrlBarWidget();

signals:
	void sigPause(bool pause);
	void sigSeek(double pos);
	void sigBackward();
	void sigForward();
	void sigStop();

public:
	void slotSetTime(int curSec);
	void slotStartPlay(int totalSec);
	void clear();

private slots:
	void slotPlayOrPause();
	void slotVolume();

private:
	bool m_isStartPlay = false;
	int m_totalTime = 0;

private:
	Ui::WCtrlBarWidgetClass ui;
};
