#include "WVideoThread.h"
#include "WDecode.h"

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

bool WVideoThread::open(AVCodecParameters *para, VideoFunc func)
{
	if (!para)
		return false;

	int ret = true;
	if (!m_decode->open(para))
	{
		cout << "video decode open failed!" << endl;
		ret = false;
	}

	m_func = func;

	return ret;
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
		//if (synpts > 0 && synpts < decode->pts)
		//{
		//	vmux.unlock();
		//	msleep(1);
		//	continue;
		//}
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
