#ifndef _WAUDIOTHREAD_H_
#define _WAUDIOTHREAD_H_

#include <QMutex>
#include <AL/al.h>
#include <AL/alc.h>
#include "WDecodeThread.h"
#include "global.h"

class WDecode;
class WAudioThread : public WDecodeThread
{
public:
	WAudioThread(QObject *parent = Q_NULLPTR);
	virtual ~WAudioThread();

public:
	virtual bool open(AVCodecParameters *para);

	virtual void close();

protected:
	void run();

private:
	void initOpenAL();
	void releaseOpenAL();
	void setALParams();

public:
	long long m_synpts = 0;

private:
	QMutex m_audioMutex;
	bool m_isPause = false;

	int m_unPlayCount = 0;

	SwrContext* m_swrContext = NULL;

	double m_oneframeduration = 0;	//计算一帧数据持续时间 ms

	long long m_IsplayBufferSize = 0;	//已经播放的队列数量
};

#endif // !_WVIDEOTHREAD_H_
