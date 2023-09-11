#include "WCtrlBarWidget.h"
#include "GlobalHelper.h"

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
}

WCtrlBarWidget::~WCtrlBarWidget()
{}

void WCtrlBarWidget::slotBackward()
{

}

void WCtrlBarWidget::slotForward()
{

}

void WCtrlBarWidget::slotPlayOrPause()
{

}

void WCtrlBarWidget::slotStop()
{

}

void WCtrlBarWidget::slotVolume()
{

}
