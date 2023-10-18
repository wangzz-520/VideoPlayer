#ifndef _WVIDEOTHREAD_H_
#define _WVIDEOTHREAD_H_

#include "WDecodeThread.h"
#include "global.h"

class WDecode;

class WVideoThread : public WDecodeThread
{
public:
	WVideoThread(QObject *parent = Q_NULLPTR);
	virtual ~WVideoThread();

public:
	virtual bool open(AVCodecParameters *para, VideoDataFunc func, 
		VideoInfoFunc infoFunc,TimeFunc timeFunc);

	virtual void setSynPts(long long pts);
	
	void setPause(bool isPause);

	//解码pts，如果接收到的解码数据pts >= seekpts return true 并且显示画面
	virtual bool repaintPts(AVPacket *pkt, long long seekpts);

	virtual void setParams(int index, double timeBase,int width,int height);

protected:
	void run();

private:
	QMutex m_videoMutex;
	bool m_isPause = false;

	VideoDataFunc m_func;

	VideoInfoFunc m_infoFunc;

	TimeFunc m_timeFunc;

	long long m_synpts = 0;

	int m_width = 0;
	int m_height = 0;
};

#endif // !_WVIDEOTHREAD_H_

