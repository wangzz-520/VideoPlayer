#include "WAudioThread.h"
#include "WDecode.h"
#include <QElapsedTimer>
#include <QDebug>

static ALuint g_buffer;
static ALuint g_source;
static ALCdevice *g_device = nullptr;
static ALCcontext *g_context = nullptr;
static const ALCchar *g_defaultDeviceName = nullptr;
static ALenum g_error;
static ALboolean g_enumeration;
static ALfloat g_listenerOri[6] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };


#define TEST_ERROR(_msg)		\
	g_error = alGetError();		\
	if (g_error != AL_NO_ERROR) {	\
		std::cout << _msg << ", " << g_error << " ohohoh\n";	\
	}

static void list_audio_devices(const ALCchar *devices)
{
	const ALCchar *device = devices, *next = devices + 1;
	size_t len = 0;

	//fprintf(stdout, "Devices list:\n");
	std::cout << "Devices list:\n";
	//fprintf(stdout, "----------\n");
	std::cout << "-------------\n";
	while (device && *device != '\0' && next && *next != '\0') {
		//fprintf(stdout, "%s\n", device);
		std::cout << device << '\n';
		len = strlen(device);
		device += (len + 1);
		next += (len + 2);
	}
	//fprintf(stdout, "----------\n");
	std::cout << "-------------\n";
}

WAudioThread::WAudioThread(QObject *parent /*= Q_NULLPTR*/)
	: WDecodeThread(parent)
{
	// 创建重采样上下文
	m_swrContext = swr_alloc();
	if (!m_swrContext)
	{
		std::cerr << "Failed to allocate resampler context." << std::endl;
	}

	initOpenAL();
	setALParams();
}

WAudioThread::~WAudioThread()
{
}

bool WAudioThread::open(AVCodecParameters *para)
{
	if (!para)
		return false;

	int ret = true;
	if (!m_decode->open(para))
	{
		cout << "audio decode open failed!" << endl;
		ret = false;
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

	m_synpts = 0;

	return ret;
}

void WAudioThread::close()
{
	WDecodeThread::close();
	if (m_swrContext)
	{
		m_audioMutex.lock();
		swr_free(&m_swrContext);
		delete m_swrContext;
		m_swrContext = NULL;
		m_audioMutex.unlock();
	}

	releaseOpenAL();
}

void WAudioThread::run()
{
	// 分配输出音频数据
	uint8_t **out_data = NULL;

	while (!m_isExit)
	{
		m_audioMutex.lock();
		if (m_isPause)
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

			//输入的样本数
			int in_nb_samples = frame->nb_samples;//1024

			//输出的样本数
			int out_linesize;
			int dst_nb_samples = av_rescale_rnd(in_nb_samples, 44100, frame->sample_rate, AV_ROUND_UP);

			av_samples_alloc_array_and_samples(&out_data, &out_linesize,2, dst_nb_samples, AV_SAMPLE_FMT_S16, 0);

			//返回每个通道输出的样本数，错误时为负值
			int sampleCount = swr_convert(m_swrContext, out_data, dst_nb_samples,
				(const uint8_t**)frame->data, in_nb_samples);

			if (sampleCount < 0)
			{
				std::cerr << "Error while resampling." << std::endl;
				break;
			}
			
			int outSize = sampleCount * 2 * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);

			std::vector<uint8_t> pcmData;
			pcmData.resize(outSize);
			memcpy(pcmData.data(), out_data[0], pcmData.size());

			m_oneframeduration = outSize * 1.0 / (16 / 8) / 2 / 44100 * 1000;

			m_synpts = m_oneframeduration * m_IsplayBufferSize;

			int data_size = av_get_bytes_per_sample(m_decode->m_pCodecCtx->sample_fmt);
			if (data_size < 0)
			{
				break;
			}

			ALuint bufferID = 0;
			alGenBuffers(1, &bufferID);

			char errorBuff[100] = {0};
			sprintf(errorBuff, "%d", bufferID);

			TEST_ERROR("alGenBuffers bufferID = "+ std::string(errorBuff));

			alBufferData(bufferID, AL_FORMAT_STEREO16, pcmData.data(), pcmData.size(), 44100);
			TEST_ERROR("alBufferData....");

			// 将缓冲区添加到OpenAL源中
			alSourceQueueBuffers(g_source, 1, &bufferID);
			TEST_ERROR("alSourceQueueBuffers....");

			// 检查已经播放的缓冲区
			ALint processed;
			alGetSourcei(g_source, AL_BUFFERS_PROCESSED, &processed);
			TEST_ERROR("get process buffer size....");

			//检查总的缓冲区数量
			ALint totalNum;
			alGetSourcei(g_source, AL_BUFFERS_QUEUED, &totalNum);
			TEST_ERROR("get queue buffer size....");
			
			ALint source_state;
			alGetSourcei(g_source, AL_SOURCE_STATE, &source_state);

			if (source_state == AL_STOPPED ||source_state == AL_PAUSED ||source_state == AL_INITIAL)
			{
				//如果没有数据,或数据播放完了  
				if (totalNum < processed || totalNum == 0 || (totalNum == 1 && processed == 1))
				{
					//停止播放  
					printf("...Audio Stop ");
					break;
				}

				if (source_state != AL_PLAYING)
				{
					alSourcePlay(g_source);
					TEST_ERROR("play voice....");
				}
			}

			while (processed > 0) {
				ALuint buffer;
				alSourceUnqueueBuffers(g_source, 1, &buffer);
				sprintf(errorBuff, "%d", buffer);
				TEST_ERROR("alSourceUnqueueBuffers...." + std::string(errorBuff));

				alDeleteBuffers(1, &buffer);

				m_oneframeduration++;
				processed--;
			}
	
			bufferID = 0;
			av_frame_free(&frame);
		}
		m_audioMutex.unlock();
	}
}

void WAudioThread::initOpenAL()
{
	g_enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
	if (g_enumeration == AL_FALSE)
	{
		std::cout << "enumeration extension not available\n";
	}

	//list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));

	if (!g_defaultDeviceName)
	{
		g_defaultDeviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
	}

	g_device = alcOpenDevice(g_defaultDeviceName);
	if (!g_device)
	{
		//fprintf(stderr, "unable to open default device\n");
		std::cout << "ohoh\n";
	}

	g_context = alcCreateContext(g_device, NULL);
	if (!alcMakeContextCurrent(g_context)) {
		std::cout << "ohoh2\n";
	}
	TEST_ERROR("make default context");
}

void WAudioThread::releaseOpenAL()
{
	alDeleteSources(1, &g_source);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(g_context);
	alcCloseDevice(g_device);
}

void WAudioThread::setALParams()
{
	///* set orientation */
	alListener3f(AL_POSITION, 0, 0, 1.0f);
	TEST_ERROR("listener position");
	alListener3f(AL_VELOCITY, 0, 0, 0);
	TEST_ERROR("listener velocity");
	alListenerfv(AL_ORIENTATION, g_listenerOri);
	TEST_ERROR("listener orientation");

	alGenSources(1, &g_source);
	TEST_ERROR("source generation");
	alSourcef(g_source, AL_PITCH, 1);
	TEST_ERROR("source pitch");
	alSourcef(g_source, AL_GAIN, 10);
	TEST_ERROR("source gain");
	alSource3f(g_source, AL_POSITION, 0, 0, 0);
	TEST_ERROR("source position");
	alSource3f(g_source, AL_VELOCITY, 0, 0, 0);
	TEST_ERROR("source velocity");
	alSourcei(g_source, AL_LOOPING, AL_FALSE);
	TEST_ERROR("source looping");
}
