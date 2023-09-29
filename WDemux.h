#ifndef _WDEMUX_H_
#define _WDEMUX_H_

#include <mutex>
#include "global.h"
/*****************************************************************//**
 * \file   WDemux.h
 * \brief  Demux
 * 
 * \author 95320
 * \date   September 2023
 *********************************************************************/

class WDemux
{
public:
	WDemux();
	virtual ~WDemux();

public:
	//open url
	virtual bool open(const char *url);
	//read packet
	virtual AVPacket *read();
	//check isAudio
	virtual bool isAudio(AVPacket *packet);

	//获取视频参数  返回的空间需要清理  avcodec_parameters_free
	virtual AVCodecParameters *videoPara();

	//获取音频参数  返回的空间需要清理 avcodec_parameters_free
	virtual AVCodecParameters *audioPara();

public:
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
	//音频样本率
	int m_sampleRate = 0;
	//音频通道数
	int m_channels = 0;

protected:
	double r2d(AVRational r);

protected:
	//解封装上下文
	AVFormatContext *m_pFormatCtx = NULL;

	AVCodecContext* m_pCodecCtx = NULL;
	AVCodecContext *m_audioCodecCtx = NULL;

	std::mutex m_mux;
};

#endif // 