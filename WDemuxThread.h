#ifndef _WDEMUXTHREAD_H_
#define _WDEMUXTHREAD_H_

#include <QThread>
#include <QMutex>
#include "global.h"

class WDemux;
class WAudioThread;
class WVideoThread;

class WDemuxThread : public QThread
{
public:
	WDemuxThread(QObject *parent = Q_NULLPTR);
	virtual ~WDemuxThread();

public:
	bool open(const char *url, VideoFunc func, TotalTimeFunc totalTimeFunc, TimeFunc timeFunc);

	void close();

	void setPause(bool isPause);

	void seek(double pos);

	void clear();

protected:
	void run();

private:
	//exit?
	bool m_isExit = false;
	//pause?
	bool m_isPause = false;

	WDemux *m_demux = nullptr;

	WAudioThread *m_audioThread = nullptr;
	WVideoThread *m_videoThread = nullptr;

	QMutex m_mutex;

	long long m_pts = 0;
};

#endif // 

