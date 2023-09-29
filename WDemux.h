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

	//��ȡ��Ƶ����  ���صĿռ���Ҫ����  avcodec_parameters_free
	virtual AVCodecParameters *videoPara();

	//��ȡ��Ƶ����  ���صĿռ���Ҫ���� avcodec_parameters_free
	virtual AVCodecParameters *audioPara();

public:
	//��Ƶindex
	int m_videoIndex = -1;
	//��Ƶindex
	int m_audioIndex = -1;
	//��Ƶ��ʱ��,��λms
	int64_t m_totalTime = 0;
	//��Ƶ����;
	int m_width = 0;
	//��Ƶ�߶�;
	int m_height = 0;
	//��Ƶ֡��;
	int m_fps = 0;
	//��Ƶ������
	int m_sampleRate = 0;
	//��Ƶͨ����
	int m_channels = 0;

protected:
	double r2d(AVRational r);

protected:
	//���װ������
	AVFormatContext *m_pFormatCtx = NULL;

	AVCodecContext* m_pCodecCtx = NULL;
	AVCodecContext *m_audioCodecCtx = NULL;

	std::mutex m_mux;
};

#endif // 