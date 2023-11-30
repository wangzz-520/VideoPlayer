#pragma once

#include <QMainWindow>
#include <QMouseEvent>
#include "ui_MainWindow.h"
#include "global.h"

class WDemuxThread;
class SlideAnimationWidget;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void slotActionOpen();
	void slotActionOpenMore();
	void slotSetPause(bool isPause);
	void slotSeek(double pos);
	void slotVolumn(double pos);

protected:
	//Ë«»÷È«ÆÁ
	void mouseDoubleClickEvent(QMouseEvent *e);
	virtual void resizeEvent(QResizeEvent *event);
	virtual void showEvent(QShowEvent *event);

private slots:
	void slotShowVideo(const QString &fileName);
	void slotBackward();
	void slotForward();
	void slotStop();

private:
	Ui::MainWindowClass ui;

private:
	WDemuxThread *m_demuxThread = nullptr;

	SlideAnimationWidget *m_animationWidget = nullptr;

	bool m_isInit = false;
	bool m_isStop = false;	//ÊÇ·ñµã»÷Í£Ö¹²¥·Å
};
