#include "WVideoThread.h"
#include "WDecode.h"
#include <QDebug>
#include <QElapsedTimer>


WVideoThread::WVideoThread(QObject *parent /*= Q_NULLPTR*/)
	: WDecodeThread(parent)
{

}

WVideoThread::~WVideoThread()
{
	m_func = 0;
	m_isExit = true;
	wait();
}

bool WVideoThread::open(AVCodecParameters *para, VideoFunc func, TimeFunc timeFunc)
{
	if (!para)
		return false;

	//清理缓冲队列
	clear();

	m_videoMutex.lock();
	m_synpts = 0;
	int ret = true;
	if (!m_decode->open(para))
	{
		m_videoMutex.unlock();
		cout << "video decode open failed!" << endl;
		ret = false;

		return ret;
	}

	m_func = func;
	m_timeFunc = timeFunc;

	m_videoMutex.unlock();

	return ret;
}

void WVideoThread::setSynPts(long long pts)
{
	m_synpts = pts;
}

void WVideoThread::setPause(bool isPause)
{
	m_videoMutex.lock();
	m_isPause = isPause;
	m_videoMutex.unlock();
}

bool WVideoThread::repaintPts(AVPacket *pkt, long long seekpts)
{
	m_videoMutex.lock();
	bool re = m_decode->send(pkt);
	if (!re)
	{
		m_videoMutex.unlock();
		return true; //表示结束解码
	}
	AVFrame *frame = m_decode->recv();
	if (!frame)
	{
		m_videoMutex.unlock();
		return false;
	}
	uint8_t* buffer = NULL;
	//到达位置
	if (m_decode->m_pts >= seekpts)
	{
		if (m_func)
		{
			int width = frame->width;
			int height = frame->height;

			int ySize = frame->width * frame->height;
			int uvSize = ySize / 4;
			int totalSize = ySize + 2 * uvSize;
	
			buffer = new uint8_t[totalSize];
			memset(buffer, 0, sizeof(buffer));
			// 拷贝YUV420P数据到缓冲区
			memcpy(buffer, frame->data[0], ySize); // 拷贝Y分量
			memcpy(buffer + ySize, frame->data[1], uvSize); // 拷贝U分量
			memcpy(buffer + ySize + uvSize, frame->data[2], uvSize); // 拷贝V分量
			m_func(buffer, width, height);
		}
			
		m_videoMutex.unlock();
		return true;
	}

	av_frame_free(&frame);
	delete[]buffer;
	m_videoMutex.unlock();
	return false;
}

void WVideoThread::run()
{
	uint8_t* buffer = NULL;
	while (!m_isExit)
	{
		m_videoMutex.lock();
		if (m_isPause)
		{
			m_videoMutex.unlock();
			msleep(5);
			continue;
		}

		//音视频同步
		if (m_synpts > 0 && m_synpts < m_decode->m_pts)
		{
			m_videoMutex.unlock();
			msleep(1);
			continue;
		}
		
		AVPacket *pkt = pop();
		
		bool ret = m_decode->send(pkt);
		if (!ret)
		{
			m_videoMutex.unlock();
			msleep(1);
			continue;
		}
		//一次send 多次recv
		while (!m_isExit)
		{
			AVFrame * frame = m_decode->recv();
			if (!frame)
				break;

			switch (frame->format)
			{
			case AV_PIX_FMT_YUV420P:
			{
				int width = frame->width;
				int height = frame->height;

				int ySize = frame->width * frame->height;
				int uvSize = ySize / 4;
				int totalSize = ySize + 2 * uvSize;

				// 创建缓冲区
				if (!buffer)
				{
					buffer = new uint8_t[totalSize];
					memset(buffer, 0, sizeof(buffer));
				}

				// 拷贝YUV420P数据到缓冲区
				memcpy(buffer, frame->data[0], ySize); // 拷贝Y分量
				memcpy(buffer + ySize, frame->data[1], uvSize); // 拷贝U分量
				memcpy(buffer + ySize + uvSize, frame->data[2], uvSize); // 拷贝V分量

				int second = frame->pts * m_timeBase;

				m_timeFunc(second);
				//发送信号，yuv数据
				m_func(buffer, width, height);

			}break;
			case AV_PIX_FMT_YUV422P:
			{
				int width = frame->width;
				int height = frame->height;

				// 确定YUV422P数据的大小
				int ySize = frame->width * frame->height;
				int uvSize = ySize / 2;
				int totalSize = ySize + 2 * uvSize;
				// 创建缓冲区
				if (!buffer)
				{
					buffer = new uint8_t[totalSize];
					memset(buffer, 0, sizeof(buffer));
				}

				// 拷贝YUV422P数据到缓冲区
				memcpy(buffer, frame->data[0], ySize); // 拷贝Y分量
				memcpy(buffer + ySize, frame->data[1], uvSize); // 拷贝U分量
				memcpy(buffer + ySize + uvSize, frame->data[2], uvSize); // 拷贝V分量

				//yuv数据
				m_func(buffer, width, height);
				//delete[]buffer;
			}break;
			case AV_PIX_FMT_YUV444P:
			{
				int width = frame->width;
				int height = frame->height;

				// 确定YUV422P数据的大小
				int ySize = frame->width * frame->height;
				int uvSize = ySize;
				int totalSize = ySize + 2 * uvSize;
				// 创建缓冲区
				if (!buffer)
				{
					buffer = new uint8_t[totalSize];
					memset(buffer, 0, sizeof(buffer));
				}

				// 拷贝YUV444P数据到缓冲区
				memcpy(buffer, frame->data[0], ySize); // 拷贝Y分量
				memcpy(buffer + ySize, frame->data[1], uvSize); // 拷贝U分量
				memcpy(buffer + ySize + uvSize, frame->data[2], uvSize); // 拷贝V分量

				//发送信号，yuv数据
				m_func(buffer, width, height);
				//delete[]buffer;
			}break;
			default:
			{
				return;
			}
			}
			av_frame_free(&frame);
		}

		m_videoMutex.unlock();
	}
	if (buffer)
		delete[]buffer;
}
