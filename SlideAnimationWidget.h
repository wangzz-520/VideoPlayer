#ifndef SLIDEANIMATIONWIDGET_H
#define SLIDEANIMATIONWIDGET_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QEvent>
#include <QRect>
#include <QStringListModel>

#include "global.h"

namespace Ui {
class SlideAnimationWidget;
}

class SlideAnimationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SlideAnimationWidget(QWidget *parent = 0);
    ~SlideAnimationWidget();

signals:
	void sigShowVideo(const QString &fileName);

public:
    void setPos(int x,int y);
	void stopAnimation();
	void addList(const QString &fileName);
	void addList(const QStringList &fileNames);
	int curIndex();
	QString getNextPlayFileName(bool isForWard);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

public slots :
    void slotSlideOutFinished();

    void slotSlideInFinished();

	void slotDoubleClicked(const QModelIndex &index);

private:
    Ui::SlideAnimationWidget *ui;

private:
    QPropertyAnimation *m_slideOutAnimation = nullptr;
    QPropertyAnimation *m_slideInAnimation = nullptr;
    bool m_bShowSideflag = false;   //显示侧边栏
    bool m_bContainMouse = false;

    int m_posX = 0;
    int m_posY = 0;
    bool m_isInit = false;

	QStringListModel *m_model = nullptr;

	QMap<int, QString> m_map; //[row,filePath]
};

#endif // SLIDEANIMATIONWIDGET_H
