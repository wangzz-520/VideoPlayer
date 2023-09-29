#ifndef _WDECODE_H_
#define _WDECODE_H_
#include "global.h"

class WDecode
{
public:
	WDecode();
	virtual ~WDecode();
public:
	//打开解码器
	virtual bool open(AVCodecParameters *para);

	virtual void close();

	//发送到解码线程，不管成功与否都释放pkt空间（对象和媒体内容）
	virtual bool send(AVPacket *pkt);

	//获取解码数据,一次send可能需要多次Recv，获取缓冲中的数据Send NULL在Recv多次
	virtual AVFrame* recv();

public:
	AVCodecContext *m_pCodecCtx = 0;

	std::mutex m_mux;
};

#endif // 

