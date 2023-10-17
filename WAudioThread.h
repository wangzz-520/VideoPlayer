#ifndef _WAUDIOTHREAD_H_
#define _WAUDIOTHREAD_H_

#include <QMutex>
#include "WDecodeThread.h"
#include "global.h"

class WAudioPlay;
class WDecode;
class WAudioThread : public WDecodeThread
{
public:
	WAudioThread(QObject *parent = Q_NULLPTR);
	virtual ~WAudioThread();

public:
	virtual bool open(AVCodecParameters *para);

	virtual void close();

	void setPause(bool isPause);

	virtual void clear();

protected:
	void run();

public:
	long long m_pts = 0;

private:
	QMutex m_audioMutex;
	bool m_isPause = false;

	int m_unPlayCount = 0;

	SwrContext* m_swrContext = NULL;

	WAudioPlay *m_play = nullptr;
};

#endif // !_WVIDEOTHREAD_H_
