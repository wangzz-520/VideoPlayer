#include "Audioplayer.h"
#include <QElapsedTimer>
#include <QDebug>

#define  M_PI 3.1415926

ALuint g_buffer;
ALuint g_source;
ALCdevice *g_device = nullptr;
ALCcontext *g_context = nullptr;
ALenum g_error;

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

Audioplayer::Audioplayer(QObject *parent)
	: QThread(parent)
{
	enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
	if (enumeration == AL_FALSE) 
	{
		std::cout << "enumeration extension not available\n";
	}

	list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));
  
	if (!defaultDeviceName) 
	{
		defaultDeviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
	}
  
	g_device = alcOpenDevice(defaultDeviceName);
	if (!g_device)
	{
		//fprintf(stderr, "unable to open default device\n");
		std::cout << "ohoh\n";
	}

	alGetError();
  
	g_context = alcCreateContext(g_device, NULL);
	if (!alcMakeContextCurrent(g_context)) {
		std::cout << "ohoh2\n";
	}
	TEST_ERROR("make default context");
  
	///* set orientation */
	alListener3f(AL_POSITION, 0, 0, 1.0f);
	TEST_ERROR("listener position");
	alListener3f(AL_VELOCITY, 0, 0, 0);
	TEST_ERROR("listener velocity");
	alListenerfv(AL_ORIENTATION, listenerOri);
	TEST_ERROR("listener orientation");

	alGenBuffers(1, &g_buffer);

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

	// 打开PCM数据文件
	//FILE *file = fopen("test.pcm", "rb");
	//if (!file) {
	//	printf("无法打开PCM数据文件\n");
	//	return;
	//}
	//// 读取PCM数据
	//char *buffer = new char[10 * 1024 * 1024];
	//int bytesRead = fread(buffer, 1,strlen(buffer), file);
	//if (bytesRead <= 0) {
	//	printf("无法读取PCM数据\n");
	//	fclose(file);
	//	return;
	//}
	//// 创建OpenAL缓冲区
	//ALuint bufferId;
	//alGenBuffers(1, &bufferId);
	//alBufferData(bufferId, AL_FORMAT_STEREO16, buffer, bytesRead, 44100);

	//alGenSources(1, &g_source);
	//alSourcei(g_source, AL_BUFFER, bufferId);
	//// 播放源
	//alSourcePlay(g_source);
	//// 等待播放完成
	//ALint sourceState;
	//do {
	//	alGetSourcei(g_source, AL_SOURCE_STATE, &sourceState);
	//} while (sourceState == AL_PLAYING);
}

Audioplayer::~Audioplayer() 
{
	/* exit context */
	alDeleteSources(1, &g_source);
	alDeleteBuffers(1, &g_buffer);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(g_context);
	alcCloseDevice(g_device);
}

Audioplayer *Audioplayer::getInstance(void)
{
	static Audioplayer instance;
	return &instance;
}

void Audioplayer::setCurrentSampleInfo(int sampleRate, int channelCount)
{
	m_sampleRate = sampleRate;
	m_channelCount = channelCount;
}

void Audioplayer::push(std::vector<uint8_t> ad)
{
	m_lstBuffer.append(ad);
}

void Audioplayer::setStart(bool start)
{
	m_isStart = start;
}

void Audioplayer::run()
{
	m_isStart = true;
	while (m_isStart)
	{
		if (!m_lstBuffer.size())
		{
			msleep(5);
			continue;
		}
		QElapsedTimer timer;
		timer.start();
		std::vector<uint8_t> audio = m_lstBuffer.takeFirst();
		play(audio);
		qDebug() << "The slow operation took" << timer.elapsed() << "milliseconds";
	}
}

void Audioplayer::play(std::vector<uint8_t> buffer)
{
	ALuint temp_buffer;
	alGenBuffers(1, &temp_buffer);
	//TEST_ERROR("temp buffer generation");

	alBufferData(temp_buffer, AL_FORMAT_MONO8, buffer.data(), buffer.size(), 44100);
	//TEST_ERROR("buffer copy");

	alSourcei(g_source, AL_BUFFER, temp_buffer);
	TEST_ERROR("buffer binding");

	alSourcePlay(g_source);
	//TEST_ERROR("source playing");

	ALint source_state;
	do {
		alGetSourcei(g_source, AL_SOURCE_STATE, &source_state);
		TEST_ERROR("source state get");
	} while (source_state == AL_PLAYING);
}
