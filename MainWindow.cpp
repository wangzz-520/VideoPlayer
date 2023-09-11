#include "MainWindow.h"
#include "GlobalHelper.h"
#include "DecodeThread.h"
#include <QFileDialog>
#include <QAction>

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
}

MainWindow::~MainWindow()
{}

void MainWindow::slotActionOpen()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
		"./",
		tr("videos (*.mp4 *.mkv *.flv)"));

	if (fileName.isEmpty())
		return;

	DecodeThread *thread = new DecodeThread(this);
	connect(thread, &DecodeThread::sigData, ui.openGLWidget
		, &WOpenGLWidget::slotReceiveVideoData);
	thread->setUrl(fileName);
	thread->start();
}
