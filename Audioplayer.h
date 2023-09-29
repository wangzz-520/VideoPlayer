#include <AL/al.h>
#include <AL/alc.h>

#include <string>
#include <cstring>
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <vector>

#include <QThread>
#include <QList>

#define g_AudioPlayThread Audioplayer::getInstance()

typedef struct  
{
	uint8_t *data;
	int size;
}AudioData;

class Audioplayer : public QThread
{
	Q_OBJECT

private:
	Audioplayer(QObject *parent = Q_NULLPTR);
	~Audioplayer();

public:
	static Audioplayer *getInstance(void);

public:
	// 设置当前的采样率、通道数目
	void setCurrentSampleInfo(int sampleRate, int channelCount);
	void push(std::vector<uint8_t> ad);
	void setStart(bool start);

	void play(std::vector<uint8_t> buffer);

protected:
	void run();

private:
	ALboolean enumeration;
	const ALCchar *devices;
	const ALCchar *defaultDeviceName = nullptr;
	int ret;
	char *bufferData;

	ALfloat listenerOri[6] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };

	//void play(AudioData data);
	//void play(std::vector<uint8_t> buffer);

	int m_sampleRate;
	int m_channelCount;

	QList<std::vector<uint8_t>> m_lstBuffer;

	bool m_isStart = false;
};
