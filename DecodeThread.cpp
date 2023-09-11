#include "DecodeThread.h"
#include <QDebug>
#include <chrono>
#include "AudioPlayThread.h"

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
	if (m_audioCodecCtx != NULL)
	{
		avcodec_close(m_audioCodecCtx);
		avcodec_free_context(&m_audioCodecCtx);
		m_audioCodecCtx = NULL;
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
	m_isStartDecode = !stop;
}


void DecodeThread::setPause(bool pause)
{
	m_isPause = pause;
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
		}
		else if (m_pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			m_audioIndex = i;
		}
	}
	if (m_videoIndex < 0)
	{
		printf("Couldn't find a video stream.\n");
		return;
	}
	if(m_audioIndex < 0)
	{
		printf("Couldn't find a video stream.\n");
	}

	/**
	*  ���޳�����ʹ��
	*  pCodecCtx = pFormatCtx->streams[videoIndex]->codec;	//ָ��AVCodecContext��ָ��
	*/
	//===================video=================
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
		return;
	}
	//�򿪽�����
	if (avcodec_open2(m_pCodecCtx, pCodec, NULL) < 0)
	{
		printf("Could not open codec.\n");
		return;
	}

	//��Ƶ��
	m_width = m_pFormatCtx->streams[m_videoIndex]->codecpar->width;

	//��Ƶ��
	m_height = m_pFormatCtx->streams[m_videoIndex]->codecpar->height;

	//��Ƶ��ʱ��s
	m_totalTime = static_cast<double>(m_pFormatCtx->duration) / AV_TIME_BASE;

	//��ȡ֡��;
	m_fps = r2d(m_pFormatCtx->streams[m_videoIndex]->avg_frame_rate);
	if (m_fps == 0)
	{
		m_fps = 25;
	}
	//===================video=================

	if (m_audioIndex >= 0)
	{
		//===================audio=================
		m_audioCodecCtx = avcodec_alloc_context3(NULL);
		if (m_audioCodecCtx == NULL)
		{
			printf("Could not allocate AVCodecContext\n");
		}
		avcodec_parameters_to_context(m_audioCodecCtx, m_pFormatCtx->streams[m_audioIndex]->codecpar);

		//ָ��AVCodec��ָ��.���ҽ�����
		AVCodec *audioCodec = avcodec_find_decoder(m_audioCodecCtx->codec_id);
		if (audioCodec == NULL)
		{
			printf("audio Codec not found pCodec\n");
			return;
		}
		//�򿪽�����
		if (avcodec_open2(m_audioCodecCtx, audioCodec, NULL) < 0)
		{
			printf("Could not open codec.\n");
			return;
		}
		//===================audio=================
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

	m_isStartDecode = true;

	emit sigStart(m_totalTime);

	//��ʼ����
	decodeStream();
}

double DecodeThread::r2d(AVRational r)
{
	return r.den == 0 ? 0 : (double)r.num / (double)r.den;
}

void DecodeThread::decodeStream()
{
	uint8_t* buffer = NULL;
	while (m_isStartDecode)
	{
		if (m_isPause)
		{
			msleep(5);
			continue;
		}

		if (av_read_frame(m_pFormatCtx, m_avpacket) >= 0)
		{
			if (m_avpacket->stream_index == m_videoIndex)
			{
				auto start = std::chrono::steady_clock::now();
				int ret = avcodec_send_packet(m_pCodecCtx, m_avpacket);
				if (ret >= 0)
				{
					ret = avcodec_receive_frame(m_pCodecCtx, m_frame);
					if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
					{
						continue;
					}
					else if (ret < 0)
					{
						continue;
					}

					switch (m_pCodecCtx->pix_fmt)
					{
					case AV_PIX_FMT_YUV420P:
					{
						int width = m_frame->width;
						int height = m_frame->height;

						int ySize = m_frame->width * m_frame->height;
						int uvSize = ySize / 4;
						int totalSize = ySize + 2 * uvSize;

						// ����������
						if (!buffer)
						{
							buffer = new uint8_t[totalSize];
							memset(buffer, 0, sizeof(buffer));
						}

						// ����YUV420P���ݵ�������
						memcpy(buffer, m_frame->data[0], ySize); // ����Y����
						memcpy(buffer + ySize, m_frame->data[1], uvSize); // ����U����
						memcpy(buffer + ySize + uvSize, m_frame->data[2], uvSize); // ����V����

						//�����źţ�yuv����
						emit sigData(buffer, width, height);
						//delete[]buffer;
					}break;
					case AV_PIX_FMT_YUV422P:
					{
						int width = m_frame->width;
						int height = m_frame->height;

						// ȷ��YUV422P���ݵĴ�С
						int ySize = m_frame->width * m_frame->height;
						int uvSize = ySize / 2;
						int totalSize = ySize + 2 * uvSize;
						// ����������
						uint8_t* buffer = new uint8_t[totalSize];
						memset(buffer, 0, sizeof(buffer));

						// ����YUV422P���ݵ�������
						memcpy(buffer, m_frame->data[0], ySize); // ����Y����
						memcpy(buffer + ySize, m_frame->data[1], uvSize); // ����U����
						memcpy(buffer + ySize + uvSize, m_frame->data[2], uvSize); // ����V����

						//�����źţ�yuv����
						emit sigData(buffer, width, height);
						//delete[]buffer;
					}break;
					case AV_PIX_FMT_YUV444P:
					{
						int width = m_frame->width;
						int height = m_frame->height;

						// ȷ��YUV422P���ݵĴ�С
						int ySize = m_frame->width * m_frame->height;
						int uvSize = ySize;
						int totalSize = ySize + 2 * uvSize;
						// ����������
						uint8_t* buffer = new uint8_t[totalSize];
						memset(buffer, 0, sizeof(buffer));

						// ����YUV444P���ݵ�������
						memcpy(buffer, m_frame->data[0], ySize); // ����Y����
						memcpy(buffer + ySize, m_frame->data[1], uvSize); // ����U����
						memcpy(buffer + ySize + uvSize, m_frame->data[2], uvSize); // ����V����

						//�����źţ�yuv����
						emit sigData(buffer, width, height);
						//delete[]buffer;
					}break;
					default:
					{
						printf("default format:%d\n", m_pCodecCtx->pix_fmt);
						return;
					}
					}
				}
				int second = m_frame->pts * av_q2d(m_pFormatCtx->streams[m_videoIndex]->time_base);

				emit sigUpdateTime(second);

				auto end = std::chrono::steady_clock::now();

				auto tt = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
				//qDebug() << "spend time = " << tt.count();
				//qDebug() << "sleep time = " << 1000 / m_fps - tt.count() / 1000;
				msleep(1000 / m_fps - tt.count() / 1000);
			}
			else if (m_avpacket->stream_index == m_audioIndex)
			{
				int ret = avcodec_send_packet(m_audioCodecCtx, m_avpacket);
				if (ret >= 0)
				{
					ret = avcodec_receive_frame(m_audioCodecCtx, m_frame);
					if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
					{
						continue;
					}
					else if (ret < 0)
					{
						continue;
					}
				}

				int data_size = av_get_bytes_per_sample(m_audioCodecCtx->sample_fmt);
				if (data_size < 0)
				{
					continue;
				}
				//for (int i = 0; i < m_frame->nb_samples; i++)
				//{
				//	for (int ch = 0; ch < m_audioCodecCtx->channels; ch++)
				//	{
				//		//fwrite(m_frame->data[ch] + data_size * i, 1, data_size, f);
				//	}
				//}
				int frame_size = 0;
				int channels = 0;
				int sample_rate = 0;

				channels = m_audioCodecCtx->channels;
				sample_rate = m_audioCodecCtx->sample_rate;
				frame_size = data_size;

				// ���㻺������С
				int buffer_size = av_samples_get_buffer_size(NULL, channels, frame_size, m_audioCodecCtx->sample_fmt, 0);
				if (buffer_size < 0) 
				{
					continue;
				}

				//uint8_t *audioBuffer = (uint8_t *)malloc(buffer_size);

				// ����������Ƶ���ݿ�����������
				//av_samples_copy(&audioBuffer, m_frame->data, 0, 0, frame_size, channels, m_audioCodecCtx->sample_fmt);

				//g_AudioPlayThread->setCurrentSampleInfo(sample_rate, frame_size, channels);
				//g_AudioPlayThread->addAudioBuffer((char*)audioBuffer, buffer_size);
			}
			av_packet_unref(m_avpacket);
		}
	}
}