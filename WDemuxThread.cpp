#include "WDemuxThread.h"
#include "WDemux.h"
#include "WAudioThread.h"
#include "WVideoThread.h"

WDemuxThread::WDemuxThread(QObject *parent /*= Q_NULLPTR*/)
	: QThread(parent)
{
	m_mutex.lock();
	if (!m_demux)
		m_demux = new WDemux();

	if (!m_audioThread)
		m_audioThread = new WAudioThread();

	if (!m_videoThread) 
		m_videoThread = new WVideoThread();

	if (m_audioThread)
		m_audioThread->start();

	if (m_videoThread)
		m_videoThread->start();

	m_mutex.unlock();
}

WDemuxThread::~WDemuxThread()
{

}

bool WDemuxThread::open(const char *url, VideoFunc func, TotalTimeFunc totalTimeFunc, TimeFunc timeFunc)
{
	if (!m_demux)
		return false;

	m_mutex.lock();

	//打开解封装
	bool ret = m_demux->open(url, totalTimeFunc);
	if (!ret)
	{
		m_mutex.unlock();
		cout << "demux->Open(url) failed!" << endl;
		return false;
	}

	//打开视频解码器和处理线程
	if (!m_videoThread->open(m_demux->videoPara(), func,timeFunc))
	{
		ret = false;
		cout << "m_videoThread->Open failed!" << endl;
	}
	//打开音频解码器和处理线程
	if (!m_audioThread->open(m_demux->audioPara()))
	{
		ret = false;
		cout << "m_audioThread->Open failed!" << endl;
	}

	m_videoThread->setParams(m_demux->m_videoIndex, m_demux->m_vTimeBase);
	m_audioThread->setParams(m_demux->m_audioIndex, m_demux->m_aTimeBase);

	m_mutex.unlock();
	return true;
}

void WDemuxThread::close()
{
	m_isExit = true;
	wait();
	if (m_audioThread)
		m_audioThread->close();
	if (m_videoThread)
		m_videoThread->close();
	m_mutex.lock();
	delete m_audioThread;
	delete m_videoThread;
	m_videoThread = NULL;
	m_audioThread = NULL;
	m_mutex.unlock();
}

void WDemuxThread::run()
{
	while (!m_isExit)
	{
		m_mutex.lock();
		if (m_isPause)
		{
			m_mutex.unlock();
			msleep(5);
			continue;
		}

		if (!m_demux)
		{
			m_mutex.unlock();
			msleep(5);
			continue;
		}

		//视频同步音频
		if (m_audioThread && m_videoThread)
		{
			m_videoThread->setSynPts(m_audioThread->m_synpts);
		}

		AVPacket *pkt = m_demux->read();
		if (!pkt)
		{
			m_mutex.unlock();
			msleep(5);
			continue;
		}
		//判断数据是音频
		if (m_demux->isAudio(pkt))
		{
			if (m_audioThread) 
				m_audioThread->push(pkt);
		}
		else //视频
		{
			if (m_videoThread)
				m_videoThread->push(pkt);
		}

		m_mutex.unlock();
		msleep(1);
	}
}
