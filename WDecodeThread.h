#ifndef _WDECODETHREAD_H_
#define _WDECODETHREAD_H_

#include <QThread>
#include <QQueue>
#include <QMutex>
#include "global.h"

class WDecode;
class WDecodeThread : public QThread
{
public:
	WDecodeThread(QObject *parent = Q_NULLPTR);
	virtual ~WDecodeThread();

public:
	virtual void push(AVPacket *pkt);

	virtual AVPacket *pop();

	virtual void close();

	virtual void setParams(int index,double timeBase);

	//清理队列
	virtual void clear();

protected:
	WDecode *m_decode = nullptr;
	bool m_isExit = false;
	QQueue <AVPacket *> m_queue;
	QMutex m_mutex;

	int m_maxList = 100;

	int m_index = -1;
	double m_timeBase = 0;
};

#endif // 

