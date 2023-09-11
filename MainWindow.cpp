#include "MainWindow.h"
#include "GlobalHelper.h"
#include "DecodeThread.h"
#include <QFileDialog>
#include <QAction>
#include "AudioPlayThread.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	//setWindowFlags(Qt::FramelessWindowHint /*| Qt::WindowSystemMenuHint*/ | Qt::WindowMinimizeButtonHint);

	//¼ÓÔØÑùÊ½
	QString qss = GlobalHelper::GetQssStr(":/qss/qss/MainWidget.qss");
	setStyleSheet(qss);

	this->setWindowTitle("WPlayer");
	this->removeToolBar(ui.mainToolBar);
	this->statusBar()->hide();

	connect(ui.actionOpen, &QAction::triggered, this, &MainWindow::slotActionOpen);

	//g_AudioPlayThread->start();
}

MainWindow::~MainWindow()
{
	if (m_thread) {
		m_thread->setStoped(true);
		m_thread->wait();
	}
}

void MainWindow::slotActionOpen()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
		"./",
		tr("videos (*.mp4 *.mkv *.flv)"));

	if (fileName.isEmpty())
		return;

	m_thread = new DecodeThread(this);
	connect(m_thread, &DecodeThread::sigData, ui.openGLWidget
		, &WOpenGLWidget::slotReceiveVideoData);
	connect(m_thread, &DecodeThread::sigUpdateTime, ui.ctrlBarWidget, &WCtrlBarWidget::slotSetTime);
	connect(m_thread, &DecodeThread::sigStart, ui.ctrlBarWidget, &WCtrlBarWidget::slotStartPlay);
	connect(ui.ctrlBarWidget, &WCtrlBarWidget::sigPause, this, &MainWindow::slotSetPause);
	m_thread->setUrl(fileName);
	m_thread->start();
}

void MainWindow::slotSetPause(bool isPause)
{
	m_thread->setPause(isPause);
}
