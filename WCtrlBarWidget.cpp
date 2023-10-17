#include "WCtrlBarWidget.h"
#include "GlobalHelper.h"

static QString playStyle = "QPushButton#btnPlayOrPause\
{\
	border-image:url(\":/image/image/play.png\");\
}";

static QString pauseStyle = "QPushButton#btnPlayOrPause\
{\
	border-image:url(\":/image/image/pause.png\");\
}";

WCtrlBarWidget::WCtrlBarWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	GlobalHelper::SetIcon(ui.btnPlayOrPause, 12, QChar(0xf04b));
	GlobalHelper::SetIcon(ui.btnStop, 12, QChar(0xf04d));
	GlobalHelper::SetIcon(ui.btnVolume, 12, QChar(0xf028));
	GlobalHelper::SetIcon(ui.btnForward, 12, QChar(0xf051));
	GlobalHelper::SetIcon(ui.btnBackward, 12, QChar(0xf048));

	ui.btnVolume->setToolTip("静音");
	ui.btnForward->setToolTip("下一个");
	ui.btnBackward->setToolTip("上一个");
	ui.btnStop->setToolTip("停止");
	ui.btnPlayOrPause->setToolTip("播放");

	QString qss = GlobalHelper::GetQssStr(":/qss/qss/WCtrlBarWidget.qss");
	setStyleSheet(qss);

	connect(ui.btnBackward, &QPushButton::clicked, this, &WCtrlBarWidget::slotBackward);
	connect(ui.btnForward, &QPushButton::clicked, this, &WCtrlBarWidget::slotForward);
	connect(ui.btnPlayOrPause, &QPushButton::clicked, this, &WCtrlBarWidget::slotPlayOrPause);
	connect(ui.btnStop, &QPushButton::clicked, this, &WCtrlBarWidget::slotStop);
	connect(ui.btnVolume, &QPushButton::clicked, this, &WCtrlBarWidget::slotVolume);

	connect(ui.playSlider, &WCustomSlider::sigCustomSliderValueChanged, this, &WCtrlBarWidget::sigSeek);
}

WCtrlBarWidget::~WCtrlBarWidget()
{}

void WCtrlBarWidget::slotSetTime(int curSec)
{
	int thh, tmm, tss;
	thh = curSec / 3600;
	tmm = (curSec % 3600) / 60;
	tss = (curSec % 60);
	QTime TotalTime2(thh, tmm, tss);

	ui.playSlider->setValue(curSec * 1.0 / m_totalTime * MAX_SLIDER_VALUE);
	ui.videoPlayTimeTimeEdit->setTime(TotalTime2);
}

void WCtrlBarWidget::slotStartPlay(int totalSec)
{
	m_totalTime = totalSec;
	m_isStartPlay = true;

	int thh, tmm, tss;
	thh = totalSec / 3600;
	tmm = (totalSec % 3600) / 60;
	tss = (totalSec % 60);
	QTime TotalTime(thh, tmm, tss);

	ui.videoTotalTimeTimeEdit->setTime(TotalTime);

	ui.btnPlayOrPause->setStyleSheet(pauseStyle);

	m_isStartPlay = true;
}

void WCtrlBarWidget::slotBackward()
{

}

void WCtrlBarWidget::slotForward()
{

}

void WCtrlBarWidget::slotPlayOrPause()
{
	m_isStartPlay = !m_isStartPlay;

	if (m_isStartPlay)
	{
		ui.btnPlayOrPause->setStyleSheet(pauseStyle);
		ui.btnPlayOrPause->setToolTip("暂停");
	}
	else
	{
		ui.btnPlayOrPause->setStyleSheet(playStyle);
		ui.btnPlayOrPause->setToolTip("播放");
	}
	
	emit sigPause(!m_isStartPlay);
}

void WCtrlBarWidget::slotStop()
{

}

void WCtrlBarWidget::slotVolume()
{

}
