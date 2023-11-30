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

void WDecodeThread::push(AVPacket *pkt)
{
	if (!pkt)
		return;

	while (!m_isExit)
	{
		m_mutex.lock();
		if (m_queue.size() < m_maxList)
		{
			m_queue.push(pkt);
			m_mutex.unlock();
			break;
		}
		m_mutex.unlock();
		msleep(1);
	}
}

AVPacket * WDecodeThread::pop()
{
	m_mutex.lock();
	if (!m_queue.size())
	{
		m_mutex.unlock();
		return NULL;
	}
	AVPacket *pkt = m_queue.front();
	m_queue.pop();
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
		AVPacket *pkt = m_queue.front();
		m_queue.pop();
		//AVPacket * pkt = m_queue.dequeue();
		if (!pkt)
			continue;

		av_packet_free(&pkt);
	}

	m_mutex.unlock();
}
