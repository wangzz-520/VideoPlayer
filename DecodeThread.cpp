#include "DecodeThread.h"

DecodeThread::DecodeThread(QObject *parent)
	: QThread(parent)
{
}

DecodeThread::~DecodeThread()
{
	if (m_pFormatCtx != NULL)
	{
		avformat_close_input(&m_pFormatCtx);
		avformat_free_context(m_pFormatCtx);
		m_pFormatCtx = NULL;
	}
	if (m_pCodecCtx != NULL)
	{
		avcodec_close(m_pCodecCtx);
		avcodec_free_context(&m_pCodecCtx);
		m_pCodecCtx = NULL;
	}
	if (m_avpacket != NULL)
	{
		av_packet_unref(m_avpacket);
		delete m_avpacket;
		m_avpacket = NULL;
	}
	if (m_frame != NULL)
	{
		av_frame_free(&m_frame);
		m_frame = NULL;
	}
}

void DecodeThread::setUrl(QString url)
{
	m_url = url;

}

void DecodeThread::setStoped(bool stop)
{
	m_isStop = stop;
}

void DecodeThread::run()
{
	//ע��������� �°汾������
	av_register_all();

	//��������Ƶ�ļ�
	if (avformat_open_input(&m_pFormatCtx, m_url.toStdString().c_str(), NULL, NULL) != 0)
	{
		printf("Couldn't open input stream.\n");
	}

	if (avformat_find_stream_info(m_pFormatCtx, NULL) < 0)
	{
		printf("Couldn't find stream information.\n");
	}

	for (int i = 0; i < m_pFormatCtx->nb_streams/*����Ƶ���ĸ���*/; i++)
	{
		//������Ƶ
		if (m_pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			m_videoIndex = i;
			break;
		}
	}
	if (m_videoIndex == -1)
	{
		printf("Couldn't find a video stream.\n");
	}

	/**
	*  ���޳�����ʹ��
	*  pCodecCtx = pFormatCtx->streams[videoIndex]->codec;	//ָ��AVCodecContext��ָ��
	*/

	m_pCodecCtx = avcodec_alloc_context3(NULL);
	if (m_pCodecCtx == NULL)
	{
		printf("Could not allocate AVCodecContext\n");
	}
	avcodec_parameters_to_context(m_pCodecCtx, m_pFormatCtx->streams[m_videoIndex]->codecpar);

	//ָ��AVCodec��ָ��.���ҽ�����
	AVCodec *pCodec = avcodec_find_decoder(m_pCodecCtx->codec_id);
	if (pCodec == NULL)
	{
		printf("Codec not found pCodec\n");
	}
	//�򿪽�����
	if (avcodec_open2(m_pCodecCtx, pCodec, NULL) < 0)
	{
		printf("Could not open codec.\n");
	}

	//��Ƶ��
	m_width = m_pFormatCtx->streams[m_videoIndex]->codecpar->width;

	//��Ƶ��
	m_height = m_pFormatCtx->streams[m_videoIndex]->codecpar->height;

	//��ȡ֡��;
	m_fps = r2d(m_pFormatCtx->streams[m_videoIndex]->avg_frame_rate);
	if (m_fps == 0)
	{
		m_fps = 25;
	}

	//��ʼ��AVPacket
	m_avpacket = new AVPacket;
	av_init_packet(m_avpacket);
	m_avpacket->data = NULL;

	//��ʼ��frame,
	m_frame = av_frame_alloc();
	if (!m_frame)
	{
		printf("av_frame_alloc fail\n");
	}

	//��ʼ����
	decodeStream();
}

double DecodeThread::r2d(AVRational r)
{
	return r.den == 0 ? 0 : (double)r.num / (double)r.den;
}

void DecodeThread::decodeStream()
{
	while (av_read_frame(m_pFormatCtx, m_avpacket) >= 0)
	{
		if (m_avpacket->stream_index == m_videoIndex)
		{
			int ret = avcodec_send_packet(m_pCodecCtx, m_avpacket);
			if (ret >= 0)
			{
				ret = avcodec_receive_frame(m_pCodecCtx, m_frame);
				if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
				{
					return;
				}
				else if (ret < 0)
				{
					return;
				}

				switch (m_pCodecCtx->pix_fmt)
				{
				case AV_PIX_FMT_YUV420P:
				{
					unsigned char* buffer = new unsigned char[(int)(m_width * m_height * 1.5)];
					int width = m_frame->width;
					int height = m_frame->height;

					int i, j, k = 0;
					for (i = 0; i < m_pCodecCtx->height; i++)
					{
						memcpy(buffer + m_pCodecCtx->width * i,
							m_frame->data[0] + m_frame->linesize[0] * i,
							m_pCodecCtx->width);
					}
					for (j = 0; j < m_pCodecCtx->height / 2; j++)
					{
						memcpy(buffer + m_pCodecCtx->width*i + m_pCodecCtx->width / 2 * j,
							m_frame->data[1] + m_frame->linesize[1] * j,
							m_pCodecCtx->width / 2);
					}
					for (k = 0; k < m_pCodecCtx->height / 2; k++)
					{
						memcpy(buffer + m_pCodecCtx->width*i + m_pCodecCtx->width / 2 * j + m_pCodecCtx->width / 2 * k,
							m_frame->data[2] + m_frame->linesize[2] * k,
							m_pCodecCtx->width / 2);
					}

					//�����źţ�yuv����
					emit sigData(buffer);
				}break;
				default:
				{
					printf("default format:%d\n", m_pCodecCtx->pix_fmt);
					return;
				}
				}
			}
		}
	}
}