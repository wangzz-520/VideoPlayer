#include "WCtrlBarWidget.h"
#include "GlobalHelper.h"

WCtrlBarWidget::WCtrlBarWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	GlobalHelper::SetIcon(ui.btnPlayOrPause, 12, QChar(0xf04b));
	GlobalHelper::SetIcon(ui.btnStop, 12, QChar(0xf04d));
	GlobalHelper::SetIcon(ui.btnVolume, 12, QChar(0xf028));
	GlobalHelper::SetIcon(ui.btnPlaylistCtrl, 12, QChar(0xf036));
	GlobalHelper::SetIcon(ui.btnForward, 12, QChar(0xf051));
	GlobalHelper::SetIcon(ui.btnBackward, 12, QChar(0xf048));
	GlobalHelper::SetIcon(ui.btnSetting, 12, QChar(0xf013));

	ui.btnPlaylistCtrl->setToolTip("�����б�");
	ui.btnSetting->setToolTip("����");
	ui.btnVolume->setToolTip("����");
	ui.btnForward->setToolTip("��һ��");
	ui.btnBackward->setToolTip("��һ��");
	ui.btnStop->setToolTip("ֹͣ");
	ui.btnPlayOrPause->setToolTip("����");

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
