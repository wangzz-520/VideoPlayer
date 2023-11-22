#include "WDecodeThread.h"
#include "WDecode.h"
#include <QDebug>

WDecodeThread::WDecodeThread(QObject *parent /*= Q_NULLPTR*/)
	: QThread(parent)
{

}

WDecodeThread::~WDecodeThread()
{
}

static int index = 0;
void WDecodeThread::push(AVPacket *pkt)
{
	if (!pkt)
		return;

	m_mutex.lock();
	m_queue.enqueue(pkt);
	m_mutex.unlock();

	//while (!m_isExit)
	//{
	//	m_mutex.lock();
	//	if (m_queue.size() < m_maxList)
	//	{
	//		m_queue.enqueue(pkt);
	//		//if (m_index == 0)
	//		//	qDebug() << "==========push video index = " << pkt->pts;
	//		m_mutex.unlock();
	//		break;
	//	}
	//	m_mutex.unlock();
	//	msleep(1);
	//}
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

void WDecodeThread::clear()
{
	m_mutex.lock();
	if(m_decode)
		m_decode->clear();
	while (!m_queue.empty())
	{
		AVPacket * pkt = m_queue.dequeue();
		if (!pkt)
			continue;

		av_packet_free(&pkt);
	}

	m_mutex.unlock();
}
