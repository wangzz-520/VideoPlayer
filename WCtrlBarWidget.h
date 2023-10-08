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

public:
	void slotSetTime(int curSec);
	void slotStartPlay(int totalSec);

private slots:
	void slotBackward();
	void slotForward();
	void slotPlayOrPause();
	void slotStop();
	void slotVolume();

private:
	bool m_isStartPlay = false;
	int m_totalTime = 0;

private:
	Ui::WCtrlBarWidgetClass ui;
};
