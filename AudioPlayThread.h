#ifndef AUDIO_PLAY_THREAD_H
#define AUDIO_PLAY_THREAD_H

#include <QThread>
#include <QObject>
#include <QAudioFormat>
#include <QAudioOutput>
#include <QMutex>
#include <QByteArray>
#define g_AudioPlayThread AudioPlayThread::getInstance()

class AudioPlayThread : public QThread
{
	Q_OBJECT

public:
	static AudioPlayThread *getInstance(void);

public:
	AudioPlayThread(QObject *parent = nullptr);
	~AudioPlayThread();

	// ----------- ���������� ----------------------------------------
	// ���õ�ǰ��PCM Buffer
	void setCurrentBuffer(QByteArray buffer);
	// �������
	void addAudioBuffer(char* pData, int len);
	// ��յ�ǰ������
	void cleanAllAudioBuffer(void);
	// ------------- End ----------------------------------------------

	// ���õ�ǰ�Ĳ����ʡ�����λ����ͨ����Ŀ
	void setCurrentSampleInfo(int sampleRate, int sampleSize, int channelCount);

	virtual void run(void) override;

	// ��ȡ��ǰ������λ�ô�С
	int getCurrentBuffIndex(void);
	// ��ȡ��ǰ��ʱ��
	int getCurrentTime(void);

	// �л�����״̬
	void playMusic(bool status);
	// ��ȡ��ǰ�Ĳ���״̬
	bool getPlayMusicStatus(void);
	// ��������
	void setCurrentVolumn(qreal volumn);
	// ��ȡ��ǰ����
	qreal getCurrentVolumn(void);

private:
	QAudioOutput *m_OutPut = nullptr;
	QIODevice *m_AudioIo = nullptr;

	QByteArray m_PCMDataBuffer;
	int m_CurrentPlayIndex = 0;

	QMutex m_Mutex;
	// ����״̬
	bool m_IsPlaying = true;

signals:
	void updatePlayStatus(void);
};

#endif