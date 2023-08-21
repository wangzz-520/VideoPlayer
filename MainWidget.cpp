#include "MainWidget.h"
#include "GlobalHelper.h"
#include "DecodeThread.h"
#include <QFileDialog>

MainWidget::MainWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	setWindowFlags(Qt::FramelessWindowHint /*| Qt::WindowSystemMenuHint*/ | Qt::WindowMinimizeButtonHint);

	//¼ÓÔØÑùÊ½
	QString qss = GlobalHelper::GetQssStr(":/qss/MainWidget.qss");
	setStyleSheet(qss);

	connect(ui.btnOpen, &QPushButton::clicked, this, &MainWidget::test);
}

MainWidget::~MainWidget()
{}

void MainWidget::test()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
		"./",
		tr("videos (*.mp4 *.mkv *.flv)"));

	DecodeThread *thread = new DecodeThread(this);
	connect(thread, &DecodeThread::sigData, ui.openGLWidget
		, &WOpenGLWidget::slotReceiveVideoData);
	thread->setUrl(fileName);
	thread->start();
}
