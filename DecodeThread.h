#pragma once

#include <QThread>
#include <QElapsedTimer>

extern "C"
{
#include "libavformat/avformat.h"
#include "libavutil/dict.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"  
};

class DecodeThread : public QThread
{
	Q_OBJECT

public:
	DecodeThread(QObject *parent);
	~DecodeThread();

Q_SIGNALS:
	void sigData(uint8_t* yuvBuffer,int width,int height);
	void sigUpdateTime(int curSec);
	void sigStart(int totalSec);

public:
	void setUrl(QString url);
	void setStoped(bool stop);
	void setPause(bool pause);

protected:
	void run();

protected:
	double r2d(AVRational r);
	void decodeStream();

private:
	QString m_url;
	bool m_isStartDecode = false;
	bool m_isPause = false;
	AVFormatContext* m_pFormatCtx = NULL;
	AVCodecContext* m_pCodecCtx = NULL;
	AVCodecContext *m_audioCodecCtx = NULL;
	AVPacket* m_avpacket = NULL;
	AVFrame *m_frame = NULL;
	//��Ƶindex
	int m_videoIndex = -1;
	//��Ƶindex
	int m_audioIndex = -1;
	//��Ƶ��ʱ��,��λms
	int64_t m_totalTime = 0;
	//��Ƶ���;
	int m_width = 0;
	//��Ƶ�߶�;
	int m_height = 0;
	//��Ƶ֡��;
	int m_fps = 0;
};