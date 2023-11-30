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

bool WVideoThread::open(AVCodecParameters *para, VideoDataFunc func,
	VideoInfoFunc infoFunc, TimeFunc timeFunc)
{
	if (!para)
		return false;

	//���������
	clear();

	m_videoMutex.lock();
	m_synpts = 0;
	int ret = true;
	//�򿪽�����
	if (!m_decode)
		m_decode = new WDecode();

	if (!m_decode->open(para))
	{
		m_videoMutex.unlock();
		cout << "video decode open failed!" << endl;
		ret = false;

		return ret;
	}

	m_func = func;
	m_timeFunc = timeFunc;
	m_infoFunc = infoFunc;

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

bool WVideoThread::repaintPts(AVPacket *pkt)
{
	m_videoMutex.lock();
	bool re = m_decode->send(pkt);
	if (!re)
	{
		m_videoMutex.unlock();
		return true; //��ʾ��������
	}
	AVFrame *frame = m_decode->recv();
	if (!frame)
	{
		m_videoMutex.unlock();
		return false;
	}
	
	if (m_func)
	{
		uint8_t* buffer = NULL;
		int width = frame->width;
		int height = frame->height;

		int ySize = frame->width * frame->height;
		int uvSize = ySize / 4;
		int totalSize = ySize + 2 * uvSize;
	
		buffer = new uint8_t[totalSize];
		memset(buffer, 0, sizeof(buffer));
		// ����YUV420P���ݵ�������
		memcpy(buffer, frame->data[0], ySize); // ����Y����
		memcpy(buffer + ySize, frame->data[1], uvSize); // ����U����
		memcpy(buffer + ySize + uvSize, frame->data[2], uvSize); // ����V����

		int second = frame->pts / 1000;
		//qDebug() << "second = " << second << "  frame->pts = " << frame->pts << "m_timeBase  = " << m_timeBase;

		m_timeFunc(second);
		m_func(buffer);

		delete[]buffer;
	}
		
	av_frame_free(&frame);
	m_videoMutex.unlock();

	return true;
}

void WVideoThread::setParams(int index, double timeBase, int width, int height)
{
	m_index = index;
	m_timeBase = timeBase;
	m_width = width;
	m_height = height;

	if (m_infoFunc)
		m_infoFunc(m_width, m_height);
}

void WVideoThread::run()
{
	qDebug() << "*****WVideoThread run";
	m_isFindKey = false;
	while (!m_isExit)
	{
		m_videoMutex.lock();
		if (!m_decode || m_isPause)
		{
			m_videoMutex.unlock();
			msleep(5);
			continue;
		}

		//����Ƶͬ��
		if (m_synpts > 0 && m_synpts < m_decode->m_pts)
		{
			m_videoMutex.unlock();
			msleep(1);
			continue;
		}

		AVPacket *pkt = pop();
		if (!pkt)
		{
			m_videoMutex.unlock();
			msleep(1);
			continue;
		}

		if (!m_isFindKey)
		{
			if (pkt->flags & AV_PKT_FLAG_KEY) // is keyframe
			{
				m_isFindKey = true;
			}
			else
			{
				av_packet_free(&pkt);
				m_videoMutex.unlock();
				msleep(1);
				continue;
			}
		}

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

				uint8_t* buffer = new uint8_t[totalSize];
				memset(buffer, 0, sizeof(buffer));

				if (width == frame->linesize[0]) //�������
				{
					// ����YUV420P���ݵ�������
					memcpy(buffer, frame->data[0], ySize); // ����Y����
					memcpy(buffer + ySize, frame->data[1], uvSize); // ����U����
					memcpy(buffer + ySize + uvSize, frame->data[2], uvSize); // ����V����
				}
				else//�ж�������
				{
					int j = 0;
					for (int i = 0; i < height; i++)
					{
						memcpy(buffer + j, frame->data[0] + i * frame->linesize[0], width);
						j += width;
					}
					for (int i = 0; i < height / 2; i++)
					{
						memcpy(buffer + j, frame->data[1] + i * frame->linesize[1], width / 2);
						j += width / 2;
					}
					for (int i = 0; i < height / 2; i++)
					{
						memcpy(buffer + j, frame->data[2] + i * frame->linesize[2], width / 2);
						j += width / 2;
					}
				}

				int second = frame->pts / 1000;
				//qDebug() << "second = " << second << "  frame->pts = " << frame->pts << "m_timeBase  = " << m_timeBase;

				m_timeFunc(second);
				//�����źţ�yuv����
				m_func(buffer);

				delete[]buffer;
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
				uint8_t* buffer = new uint8_t[totalSize];
				memset(buffer, 0, sizeof(buffer));

				if (width == frame->linesize[0]) //�������
				{
					// ����YUV422P���ݵ�������
					memcpy(buffer, frame->data[0], ySize); // ����Y����
					memcpy(buffer + ySize, frame->data[1], uvSize); // ����U����
					memcpy(buffer + ySize + uvSize, frame->data[2], uvSize); // ����V����
				}
				else
				{
					int j = 0;
					for (int i = 0; i < height; i++)
					{
						memcpy(buffer + j, frame->data[0] + i * frame->linesize[0], width);
						j += width;
					}
					for (int i = 0; i < height / 2; i++)
					{
						memcpy(buffer + j, frame->data[1] + i * frame->linesize[1], width);
						j += width;
					}
					for (int i = 0; i < height / 2; i++)
					{
						memcpy(buffer + j, frame->data[2] + i * frame->linesize[2], width);
						j += width;
					}
				}

				//yuv����
				m_func(buffer);
				delete[]buffer;
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
				uint8_t* buffer = new uint8_t[totalSize];
				memset(buffer, 0, sizeof(buffer));

				// ����YUV444P���ݵ�������
				memcpy(buffer, frame->data[0], ySize); // ����Y����
				memcpy(buffer + ySize, frame->data[1], uvSize); // ����U����
				memcpy(buffer + ySize + uvSize, frame->data[2], uvSize); // ����V����

				//�����źţ�yuv����
				m_func(buffer);
				delete[]buffer;
			}break;
			default:
			{
				return;
			}
			}
			av_frame_free(&frame);
		}

		m_videoMutex.unlock();
		msleep(20);
	}

	qDebug() << "*****WVideoThread stop";
}
