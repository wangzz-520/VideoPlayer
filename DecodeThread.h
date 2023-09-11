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
	//视频index
	int m_videoIndex = -1;
	//音频index
	int m_audioIndex = -1;
	//视频总时间,单位ms
	int64_t m_totalTime = 0;
	//视频宽度;
	int m_width = 0;
	//视频高度;
	int m_height = 0;
	//视频帧率;
	int m_fps = 0;
};