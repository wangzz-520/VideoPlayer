#include "WDecodeThread.h"
#include "WDecode.h"

WDecodeThread::WDecodeThread(QObject *parent /*= Q_NULLPTR*/)
	: QThread(parent)
{
	//´ò¿ª½âÂëÆ÷
	if (!m_decode)
		m_decode = new WDecode();
}

WDecodeThread::~WDecodeThread()
{
}

void WDecodeThread::push(AVPacket *pkt)
{
	if (!pkt)
		return;

	//while (!m_isExit)
	//{
	//	m_mutex.lock();
	//	if (m_queue.size() < m_maxList)
	//	{
	//		m_queue.enqueue(pkt);
	//		m_mutex.unlock();
	//		break;
	//	}
	//	m_mutex.unlock();
	//	msleep(1);
	//}
	m_mutex.lock();
	if (m_queue.size() < m_maxList)
	{
		m_queue.enqueue(pkt);
		m_mutex.unlock();
		return;
	}
	m_mutex.unlock();
}

AVPacket * WDecodeThread::pop()
{
	m_mutex.lock();
	if (m_queue.isEmpty())
	{
		m_mutex.unlock();
		return NULL;
	}

	AVPacket *pkt = m_queue.dequeue();
	m_mutex.unlock();
	return pkt;
}

void WDecodeThread::close()
{
	m_isExit = true;
	wait();

	m_decode->close();

	m_mutex.lock();
	delete m_decode;
	m_decode = NULL;
	m_mutex.unlock();
}

void WDecodeThread::setParams(int index, double timeBase)
{
	m_index = index;
	m_timeBase = timeBase;
}
