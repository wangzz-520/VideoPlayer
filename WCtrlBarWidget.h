#pragma once

#include <QWidget>
#include "ui_WCtrlBarWidget.h"

class WCtrlBarWidget : public QWidget
{
	Q_OBJECT

public:
	WCtrlBarWidget(QWidget *parent = nullptr);
	~WCtrlBarWidget();

private slots:
	void slotBackward();
	void slotForward();
	void slotPlayOrPause();
	void slotStop();
	void slotPlaylistCtrl();
	void slotSetting();
	void slotVolume();

private:
	Ui::WCtrlBarWidgetClass ui;
};
