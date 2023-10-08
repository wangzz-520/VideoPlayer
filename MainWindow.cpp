#include "MainWindow.h"
#include "GlobalHelper.h"
#include <QFileDialog>
#include <QAction>
#include "WDemuxThread.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	//������ʽ
	QString qss = GlobalHelper::GetQssStr(":/qss/qss/MainWidget.qss");
	setStyleSheet(qss);

	this->setWindowTitle("WPlayer");
	this->removeToolBar(ui.mainToolBar);
	this->statusBar()->hide();

	connect(ui.actionOpen, &QAction::triggered, this, &MainWindow::slotActionOpen);
	connect(ui.ctrlBarWidget, &WCtrlBarWidget::sigPause, this, &MainWindow::slotSetPause);

	if (!m_demuxThread)
	{
		m_demuxThread = new WDemuxThread(this);
		m_demuxThread->start();
	}
}

MainWindow::~MainWindow()
{
	if (m_demuxThread)
	{
		m_demuxThread->close();
		delete m_demuxThread;
	}
	
}

void MainWindow::slotActionOpen()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
		"./",
		tr("videos (*.mp4 *.mkv *.flv)"));

	if (fileName.isEmpty())
		return;

	//m_thread = new DecodeThread(this);
	//connect(m_thread, &DecodeThread::sigData, ui.openGLWidget
	//	, &WOpenGLWidget::slotReceiveVideoData);
	//connect(m_thread, &DecodeThread::sigUpdateTime, ui.ctrlBarWidget, &WCtrlBarWidget::slotSetTime);
	//connect(m_thread, &DecodeThread::sigStart, ui.ctrlBarWidget, &WCtrlBarWidget::slotStartPlay);

	//m_thread->setUrl(fileName);
	//m_thread->start();

	VideoFunc videoFunc = std::bind(&WOpenGLWidget::slotReceiveVideoData, ui.openGLWidget,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	TotalTimeFunc totalTimeFunc = std::bind(&WCtrlBarWidget::slotStartPlay, ui.ctrlBarWidget,
		std::placeholders::_1);

	TimeFunc timeFunc = std::bind(&WCtrlBarWidget::slotSetTime, ui.ctrlBarWidget,
		std::placeholders::_1);

	m_demuxThread->open(fileName.toStdString().c_str(), videoFunc, totalTimeFunc, timeFunc);
	
}

void MainWindow::slotSetPause(bool isPause)
{
}
