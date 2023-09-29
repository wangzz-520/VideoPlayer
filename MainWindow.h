#pragma once

#include <QMainWindow>
#include "ui_MainWindow.h"
#include "global.h"

class DecodeThread;
class AudioPlayThread;
class Audioplayer;
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

private:
	Ui::MainWindowClass ui;

private:
	DecodeThread *m_thread = nullptr;

	WDemuxThread *m_demuxThread = nullptr;


};
