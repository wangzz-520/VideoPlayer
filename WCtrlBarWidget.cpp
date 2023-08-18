#include "WCtrlBarWidget.h"

WCtrlBarWidget::WCtrlBarWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.btnBackward, &QPushButton::clicked, this, &WCtrlBarWidget::slotBackward);
	connect(ui.btnForward, &QPushButton::clicked, this, &WCtrlBarWidget::slotForward);
	connect(ui.btnPlaylistCtrl, &QPushButton::clicked, this, &WCtrlBarWidget::slotPlaylistCtrl);
	connect(ui.btnPlayOrPause, &QPushButton::clicked, this, &WCtrlBarWidget::slotPlayOrPause);
	connect(ui.btnSetting, &QPushButton::clicked, this, &WCtrlBarWidget::slotSetting);
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

void WCtrlBarWidget::slotPlaylistCtrl()
{

}

void WCtrlBarWidget::slotSetting()
{

}

void WCtrlBarWidget::slotVolume()
{

}
