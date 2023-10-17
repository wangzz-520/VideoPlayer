#pragma once

#include <QMainWindow>
#include "ui_MainWindow.h"
#include "global.h"


class WDemuxThread;


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void slotActionOpen();
	void slotSetPause(bool isPause);
	void slotSeek(double pos);

private:
	Ui::MainWindowClass ui;

private:
	WDemuxThread *m_demuxThread = nullptr;

};
