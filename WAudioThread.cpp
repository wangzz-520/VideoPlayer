#include "WAudioThread.h"
#include "WDecode.h"
#include "WAudioPlay.h"
#include <QElapsedTimer>
#include <QDebug>
#include <qmath.h>


WAudioThread::WAudioThread(QObject *parent /*= Q_NULLPTR*/)
	: WDecodeThread(parent)
{
	if (!m_play)
		m_play = WAudioPlay::getInstance();
}

WAudioThread::~WAudioThread()
{
}

bool WAudioThread::open(AVCodecParameters *para)
{
	if (!para)
		return false;

	clear();

	m_audioMutex.lock();

	int ret = true;

	if (!m_play->open())
	{
		cout << "WAudioPlay open failed!" << endl;
		ret = false;
	}

	//打开解码器
	if (!m_decode)
		m_decode = new WDecode();

	if (!m_decode->open(para))
	{
		cout << "audio decode open failed!" << endl;
		ret = false;
	}

	if (m_swrContext)
	{
		swr_close(m_swrContext);
		swr_free(&m_swrContext);
	}

	// 创建重采样上下文
	m_swrContext = swr_alloc();
	if (!m_swrContext)
	{
		std::cerr << "Failed to allocate resampler context." << std::endl;
	}

	// 设置重采样参数
	m_swrContext = swr_alloc_set_opts(NULL,												 //ctx
		AV_CH_LAYOUT_STEREO,																	//输出channel布局
		AV_SAMPLE_FMT_S16,																		 //输出的采样格式
		44100,																									//采样率
		av_get_default_channel_layout(m_decode->m_pCodecCtx->channels),	//输入channel布局
		m_decode->m_pCodecCtx->sample_fmt,														//输入的采样格式
		m_decode->m_pCodecCtx->sample_rate,														//输入的采样率
		0, NULL);

	// 初始化重采样上下文
	if (swr_init(m_swrContext) < 0)
	{
		std::cerr << "Failed to initialize resampler context." << std::endl;
		swr_free(&m_swrContext);
	}

	m_pts = 0;

	m_audioMutex.unlock();

	return ret;
}

void WAudioThread::close()
{
	WDecodeThread::close();
	if (m_swrContext)
	{
		m_audioMutex.lock();
		swr_close(m_swrContext);
		swr_free(&m_swrContext);
		m_swrContext = NULL;
		m_audioMutex.unlock();
	}
}

void WAudioThread::setPause(bool isPause)
{
	m_audioMutex.lock();
	m_isPause = isPause;
	if (m_play)
		m_play->setPause(isPause);

	m_audioMutex.unlock();
}

void WAudioThread::clear()
{
	WDecodeThread::clear();
	m_audioMutex.lock();
	if (m_play)
		m_play->clear();
	m_audioMutex.unlock();
}

void WAudioThread::run()
{
	qDebug() << "*****WAudioThread run";
	// 分配输出音频数据
	uint8_t **out_data = NULL;

	while (!m_isExit)
	{
		m_audioMutex.lock();
		if (m_isPause || !m_decode)
		{
			m_audioMutex.unlock();
			msleep(5);
			continue;
		}

		AVPacket *pkt = pop();
		bool ret = m_decode->send(pkt);
		if (!ret)
		{
			m_audioMutex.unlock();
			msleep(1);
			continue;
		}
		//一次send 多次recv
		while (!m_isExit)
		{
			AVFrame * frame = m_decode->recv();
			if (!frame)
				break;

			m_pts = m_decode->m_pts - m_play->getNoPlayMs();

			//输入的样本数
			int in_nb_samples = frame->nb_samples;//1024

			//输出的样本数
			int out_linesize;
			int dst_nb_samples = av_rescale_rnd(in_nb_samples, 44100, frame->sample_rate, AV_ROUND_UP);

			av_samples_alloc_array_and_samples(&out_data, &out_linesize,2, dst_nb_samples, AV_SAMPLE_FMT_S16, 0);

			//返回每个通道输出的样本数，错误时为负值
			int sampleCount = swr_convert(m_swrContext, out_data, dst_nb_samples,
				(const uint8_t**)frame->data, in_nb_samples);

			if (sampleCount <= 0)
			{
				std::cerr << "Error while resampling." << std::endl;
				break;
			}
			
			int outSize = sampleCount * 2 * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
			//播放音频
			while (!m_isExit)
			{
				if (outSize <= 0)
					break;
				//缓冲未播完，空间不够
				if (m_play->getFree() < outSize)
				{
					msleep(1);
					continue;
				}
				m_play->write(out_data[0], outSize);
				break;
			}

			av_frame_free(&frame);
			av_free(out_data[0]);
			break;
		}
		m_audioMutex.unlock();
		msleep(5);
	}

	qDebug() << "*****WAudioThread stop";
}