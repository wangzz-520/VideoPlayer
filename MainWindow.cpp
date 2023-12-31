#include "MainWindow.h"
#include "GlobalHelper.h"
#include <QFileDialog>
#include <QAction>
#include "WDemuxThread.h"
#include "SlideAnimationWidget.h"
#include "WAudioPlay.h"
#include <QAudio>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	//加载样式
	//QString qss = GlobalHelper::GetQssStr(":/qss/qss/MainWidget.qss");
	//setStyleSheet(qss);

	this->setWindowTitle("WPlayer");
	this->removeToolBar(ui.mainToolBar);
	this->statusBar()->hide();
	this->setMinimumSize(800, 600);

	connect(ui.actionOpen, &QAction::triggered, this, &MainWindow::slotActionOpen);
	connect(ui.actionOpenMore, &QAction::triggered, this, &MainWindow::slotActionOpenMore);
	connect(ui.ctrlBarWidget, &WCtrlBarWidget::sigPause, this, &MainWindow::slotSetPause);
	connect(ui.ctrlBarWidget, &WCtrlBarWidget::sigSeek, this, &MainWindow::slotSeek);
	connect(ui.ctrlBarWidget, &WCtrlBarWidget::sigBackward, this, &MainWindow::slotBackward);
	connect(ui.ctrlBarWidget, &WCtrlBarWidget::sigForward, this, &MainWindow::slotForward);
	connect(ui.ctrlBarWidget, &WCtrlBarWidget::sigStop, this, &MainWindow::slotStop);
	connect(ui.ctrlBarWidget, &WCtrlBarWidget::sigVolumn, this, &MainWindow::slotVolumn);

	if (!m_demuxThread)
	{
		m_demuxThread = new WDemuxThread(this);
		m_demuxThread->start();
	}

	m_animationWidget = new SlideAnimationWidget(this);
	connect(m_animationWidget, &SlideAnimationWidget::sigShowVideo, this, &MainWindow::slotShowVideo);
	m_animationWidget->setPos(this->width()-POS_X, POS_Y);
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

	m_animationWidget->addList(fileName);
	slotShowVideo(fileName);
}

void MainWindow::slotActionOpenMore()
{
	QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open File"),
		"./",
		tr("videos (*.mp4 *.mkv *.flv)"));

	if (fileNames.isEmpty())
		return;

	m_animationWidget->addList(fileNames);
	slotShowVideo(fileNames.at(0));
}

void MainWindow::slotSetPause(bool isPause)
{
	if (m_animationWidget->getCurPlayFileName().isEmpty())
		return;

	if (!m_isStop)
	{
		if (m_demuxThread)
			m_demuxThread->setPause(isPause);

	}
	else
	{
		m_isStop = false;
		slotShowVideo(m_animationWidget->getCurPlayFileName());
	}
}

void MainWindow::slotSeek(double pos)
{
	if (m_demuxThread)
		m_demuxThread->seek(pos);
}

void MainWindow::slotVolumn(double pos)
{
	pos *= 100;
	qreal linearVolume = QAudio::convertVolume(pos  / qreal(100.0),
		QAudio::LogarithmicVolumeScale,
		QAudio::LinearVolumeScale);

	//qDebug() << "pos = " << pos << " linearVolume = " << linearVolume << " qRound(linearVolume * 100) = " << qRound(linearVolume * 100);

	WAudioPlay::getInstance()->setVolume(qRound(linearVolume * 100));
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *e)
{
	if (isFullScreen())
	{
		ui.ctrlBarWidget->show();
		m_animationWidget->show();
		this->menuBar()->show();
		this->showNormal();
	}
	else
	{
		ui.ctrlBarWidget->hide();
		m_animationWidget->hide();
		this->menuBar()->hide();
		this->showFullScreen();
	}	
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
	m_animationWidget->stopAnimation();
	m_animationWidget->setPos(this->width() - POS_X, POS_Y);
	m_animationWidget->setFixedHeight(event->size().height()-90);
}

void MainWindow::showEvent(QShowEvent *event)
{
	Q_UNUSED(event);

	if (m_isInit)
		return;

	setWindowState(Qt::WindowFullScreen);
	showNormal();

	m_isInit = true;
}

void MainWindow::slotShowVideo(const QString &fileName)
{
	VideoDataFunc videoFunc = std::bind(&WOpenGLWidget::slotReceiveVideoData, ui.openGLWidget,
		std::placeholders::_1);

	VideoInfoFunc videoInfoFunc = std::bind(&WOpenGLWidget::slotOpenVideo, ui.openGLWidget,
		std::placeholders::_1, std::placeholders::_2);

	TotalTimeFunc totalTimeFunc = std::bind(&WCtrlBarWidget::slotStartPlay, ui.ctrlBarWidget,
		std::placeholders::_1);

	TimeFunc timeFunc = std::bind(&WCtrlBarWidget::slotSetTime, ui.ctrlBarWidget,
		std::placeholders::_1);

	m_demuxThread->open(fileName.toStdString().c_str(), videoFunc, videoInfoFunc, totalTimeFunc, timeFunc);

	if (!m_demuxThread->isRunning())
		m_demuxThread->start();
}
/**
 * 上一个.
 * 
 */
void MainWindow::slotBackward()
{
	QString fileName = m_animationWidget->getNextPlayFileName(false);
	if(!fileName.isEmpty())
		slotShowVideo(fileName);
}

/**
 * 下一个.
 * 
 */
void MainWindow::slotForward()
{
	QString fileName = m_animationWidget->getNextPlayFileName(true);
	if (!fileName.isEmpty())
		slotShowVideo(fileName);
}

void MainWindow::slotStop()
{
	if (m_demuxThread)
	{
		m_demuxThread->close();
	}

	ui.openGLWidget->clear();
	ui.ctrlBarWidget->clear();

	m_isStop = true;
}
