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

static QPalette colorTheme(const QColor& base)
{
	QPalette palette;
	palette.setColor(QPalette::Base, base);
	palette.setColor(QPalette::Window, base.darker(150));
	palette.setColor(QPalette::Mid, base.darker(110));
	palette.setColor(QPalette::Light, base.lighter(170));
	palette.setColor(QPalette::Dark, base.darker(170));
	palette.setColor(QPalette::Text, base.darker(200).lighter(800));
	palette.setColor(QPalette::WindowText, base.darker(200));

	return palette;
}

WCtrlBarWidget::WCtrlBarWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.btnVolume->setToolTip("静音");
	ui.btnForward->setToolTip("下一个");
	ui.btnBackward->setToolTip("上一个");
	ui.btnStop->setToolTip("停止");
	ui.btnPlayOrPause->setToolTip("播放");

	setStyleSheet(GlobalHelper::GetQssStr(":/qss/qss/WCtrlBarWidget.qss"));

	//setPalette(colorTheme(QColor(Qt::darkGray).darker(70)));

	connect(ui.btnBackward, &QPushButton::clicked, this, &WCtrlBarWidget::sigBackward);
	connect(ui.btnForward, &QPushButton::clicked, this, &WCtrlBarWidget::sigForward);
	connect(ui.btnPlayOrPause, &QPushButton::clicked, this, &WCtrlBarWidget::slotPlayOrPause);
	connect(ui.btnStop, &QPushButton::clicked, this, &WCtrlBarWidget::sigStop);
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

void WCtrlBarWidget::clear()
{
	QTime TotalTime;
	ui.playSlider->setValue(0);
	ui.videoPlayTimeTimeEdit->setTime(TotalTime);
	ui.videoPlayTimeTimeEdit->setTime(TotalTime);
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

void WCtrlBarWidget::slotVolume()
{

}
