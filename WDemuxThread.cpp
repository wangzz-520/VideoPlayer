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

bool WDemuxThread::open(const char *url, VideoFunc func)
{
	if (!m_demux)
		return false;

	m_mutex.lock();

	//�򿪽��װ
	bool ret = m_demux->open(url);
	if (!ret)
	{
		m_mutex.unlock();
		cout << "demux->Open(url) failed!" << endl;
		return false;
	}

	//����Ƶ�������ʹ����߳�
	if (!m_videoThread->open(m_demux->videoPara(), func))
	{
		ret = false;
		cout << "m_videoThread->Open failed!" << endl;
	}
	//����Ƶ�������ʹ����߳�
	if (!m_audioThread->open(m_demux->audioPara()))
	{
		ret = false;
		cout << "m_audioThread->Open failed!" << endl;
	}

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
		if (m_isPause)
		{
			msleep(5);
			continue;
		}

		AVPacket *pkt = m_demux->read();
		if (!pkt)
		{
			msleep(5);
			continue;
		}
		//�ж���������Ƶ
		if (m_demux->isAudio(pkt))
		{
			if (m_audioThread)
				m_audioThread->push(pkt);
		}
		else //��Ƶ
		{
			if (m_videoThread)
				m_videoThread->push(pkt);
		}

		msleep(1);
	}
}