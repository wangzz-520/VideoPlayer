#include "SlideAnimationWidget.h"
#include "ui_SlideAnimationWidget.h"
#include <QAbstractItemView>
#include <QListView>
#include <QMouseEvent>

SlideAnimationWidget::SlideAnimationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SlideAnimationWidget)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow | Qt::WindowStaysOnTopHint);

    m_slideOutAnimation = new QPropertyAnimation(this,"geometry");
    connect(m_slideOutAnimation,&QPropertyAnimation::finished,this,&SlideAnimationWidget::slotSlideOutFinished);
    m_slideOutAnimation->setEasingCurve(QEasingCurve::OutSine);
    m_slideOutAnimation->setDuration(1300);

    m_slideInAnimation = new QPropertyAnimation(this,"geometry");
    connect(m_slideInAnimation,&QPropertyAnimation::finished,this,&SlideAnimationWidget::slotSlideInFinished);
    m_slideInAnimation->setEasingCurve(QEasingCurve::InSine);
    m_slideInAnimation->setDuration(1300);

	connect(ui->listView, &QListView::doubleClicked, this, &SlideAnimationWidget::slotDoubleClicked);

    ui->stackedWidget->setCurrentIndex(0);

    ui->label->installEventFilter(this);
    ui->controlWidget->installEventFilter(this);

    this->setMaximumWidth(SLIDE_MIN_WIDTH);

	m_model = new QStringListModel(this);
	ui->listView->setModel(m_model);

	setStyleSheet(GlobalHelper::GetQssStr(":/qss/qss/SlideAnimationWidget.qss"));
}

SlideAnimationWidget::~SlideAnimationWidget()
{
    delete ui;
}

void SlideAnimationWidget::setPos(int x, int y)
{
    m_posX = x;
    m_posY = y;

    move(x,y);
}

void SlideAnimationWidget::stopAnimation()
{
	if (m_slideOutAnimation->state() == QAbstractAnimation::Running)
		m_slideOutAnimation->finished();
	if (m_slideInAnimation->state() == QAbstractAnimation::Running)
		m_slideInAnimation->finished();
}

void SlideAnimationWidget::addList(const QString &fileName)
{
	QStringList stringList = m_model->stringList();
	stringList << fileName;

	m_map.clear();

	QStringList nameList;
	for (int i = 0; i < stringList.size(); i++)
	{
		QStringList ls = stringList.at(i).split('/');
		nameList << ls.at(ls.size() - 1);
		m_map.insert(i, stringList.at(i));
	}

	m_model->setStringList(nameList);
}

void SlideAnimationWidget::addList(const QStringList &fileNames)
{
	QStringList stringList = m_model->stringList();
	stringList << fileNames;

	m_map.clear();

	QStringList nameList;
	for (int i = 0; i < stringList.size(); i++)
	{
		QStringList ls= stringList.at(i).split('/');
		nameList << ls.at(ls.size() - 1);
		m_map.insert(i, stringList.at(i));
	}

	m_model->setStringList(nameList);
	ui->listView->setCurrentIndex(m_model->index(0));
}


int SlideAnimationWidget::curIndex()
{
	return ui->listView->currentIndex().row();
}

QString SlideAnimationWidget::getNextPlayFileName(bool isForWard)
{
	int rows = m_model->rowCount();
	if (!rows)
		return QString();
	int row = ui->listView->currentIndex().row();

	//下一个
	if (isForWard)
	{
		if (row >= rows - 1)
		{
			QModelIndex modelIndex = m_model->index(0);
			ui->listView->setCurrentIndex(modelIndex);
			return m_map.value(0);
		}
		else
		{
			QModelIndex modelIndex = m_model->index(row + 1);
			ui->listView->setCurrentIndex(modelIndex);
			return m_map.value(row + 1);
		}
	}
	else//上一个
	{
		if (row <= 0)
		{
			QModelIndex modelIndex = m_model->index(rows - 1);
			ui->listView->setCurrentIndex(modelIndex);
			return m_map.value(rows - 1);
		}
		else
		{
			QModelIndex modelIndex = m_model->index(row - 1);
			ui->listView->setCurrentIndex(modelIndex);
			return m_map.value(row - 1);
		}
	}
}

bool SlideAnimationWidget::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui->label)
    {
        //鼠标进入的时候
        if (event->type() == QEvent::Enter &&
                ui->stackedWidget->currentIndex() == 0 &&
                !m_bShowSideflag)
        {
            if(m_slideOutAnimation->state() == QAbstractAnimation::Running)
                return true;

            //qDebug()<<"Enter";

            this->setMaximumWidth(SLIDE_MAX_WIDTH);
            m_slideOutAnimation->setStartValue(QRect(m_posX,m_posY,SLIDE_MIN_WIDTH,this->height()));
            m_slideOutAnimation->setEndValue(QRect(parentWidget()->width()- SLIDE_MAX_WIDTH,m_posY,SLIDE_MAX_WIDTH,this->height()));
            m_slideOutAnimation->start();

            ui->stackedWidget->setCurrentIndex(1);

            m_bShowSideflag = true;
            return true;
        }

        return false;//别的事件会传给label对象
    }
    else if(obj == ui->controlWidget)
    {
        //鼠标离开的时候
        if (event->type() == QEvent::Leave &&
                ui->stackedWidget->currentIndex() == 1 &&
                m_bShowSideflag  && !m_bContainMouse)
        {
            if(m_slideInAnimation->state() == QAbstractAnimation::Running)
                return true;

            //qDebug()<<"Leave";
            m_slideInAnimation->setStartValue(QRect(parentWidget()->width() - SLIDE_MAX_WIDTH,m_posY,SLIDE_MAX_WIDTH,this->height()));
            m_slideInAnimation->setEndValue(QRect(m_posX,m_posY,SLIDE_MIN_WIDTH,this->height()));
            m_slideInAnimation->start();

            m_bShowSideflag = false;
            return true;
        }

        return false;//别的事件会传给label对象
    }

    // standard event processing
    return QWidget::eventFilter(obj, event);
}

void SlideAnimationWidget::slotSlideOutFinished()
{

}

void SlideAnimationWidget::slotSlideInFinished()
{
    this->setMaximumWidth(SLIDE_MIN_WIDTH);
    ui->stackedWidget->setCurrentIndex(0);
}

void SlideAnimationWidget::slotDoubleClicked(const QModelIndex &index)
{
	emit sigShowVideo(m_map.value(m_model->data(index).toInt()));
	ui->listView->setCurrentIndex(index);
}