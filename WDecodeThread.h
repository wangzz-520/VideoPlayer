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

protected:
	void run();

protected:
	WDecode *m_decode = nullptr;
	bool m_isExit = false;
	QQueue <AVPacket *> m_queue;
	QMutex m_mutex;

	int m_maxList = 100;
};

#endif // 

