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

		//����Ƶͬ��
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
		//һ��send ���recv
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

				// ����������
				if (!buffer)
				{
					buffer = new uint8_t[totalSize];
					memset(buffer, 0, sizeof(buffer));
				}

				// ����YUV420P���ݵ�������
				memcpy(buffer, frame->data[0], ySize); // ����Y����
				memcpy(buffer + ySize, frame->data[1], uvSize); // ����U����
				memcpy(buffer + ySize + uvSize, frame->data[2], uvSize); // ����V����

				//�����źţ�yuv����
				m_func(buffer, width, height);
			}break;
			case AV_PIX_FMT_YUV422P:
			{
				int width = frame->width;
				int height = frame->height;

				// ȷ��YUV422P���ݵĴ�С
				int ySize = frame->width * frame->height;
				int uvSize = ySize / 2;
				int totalSize = ySize + 2 * uvSize;
				// ����������
				if (!buffer)
				{
					buffer = new uint8_t[totalSize];
					memset(buffer, 0, sizeof(buffer));
				}

				// ����YUV422P���ݵ�������
				memcpy(buffer, frame->data[0], ySize); // ����Y����
				memcpy(buffer + ySize, frame->data[1], uvSize); // ����U����
				memcpy(buffer + ySize + uvSize, frame->data[2], uvSize); // ����V����

				//yuv����
				m_func(buffer, width, height);
				//delete[]buffer;
			}break;
			case AV_PIX_FMT_YUV444P:
			{
				int width = frame->width;
				int height = frame->height;

				// ȷ��YUV422P���ݵĴ�С
				int ySize = frame->width * frame->height;
				int uvSize = ySize;
				int totalSize = ySize + 2 * uvSize;
				// ����������
				if (!buffer)
				{
					buffer = new uint8_t[totalSize];
					memset(buffer, 0, sizeof(buffer));
				}

				// ����YUV444P���ݵ�������
				memcpy(buffer, frame->data[0], ySize); // ����Y����
				memcpy(buffer + ySize, frame->data[1], uvSize); // ����U����
				memcpy(buffer + ySize + uvSize, frame->data[2], uvSize); // ����V����

				//�����źţ�yuv����
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
