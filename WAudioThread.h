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

private:
	QMutex m_audioMutex;
	bool m_isPause = false;

	SwrContext* m_swrContext = NULL;
};

#endif // !_WVIDEOTHREAD_H_
