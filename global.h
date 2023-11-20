#ifndef _GLOBAL_H_
#define _GLOBAL_H_

extern "C" {
#include "libavutil/avstring.h"
#include "libavutil/channel_layout.h"
#include "libavutil/eval.h"
#include "libavutil/mathematics.h"
#include "libavutil/pixdesc.h"
#include "libavutil/imgutils.h"
#include "libavutil/dict.h"
#include "libavutil/fifo.h"
#include "libavutil/parseutils.h"
#include "libavutil/samplefmt.h"
#include "libavutil/time.h"
#include "libavutil/bprint.h"
#include "libavutil/opt.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavfilter/avfilter.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libavutil/opt.h"
#include "libavutil/imgutils.h"  
#include "libavcodec/avfft.h"
#include "libswresample/swresample.h"
}

using namespace std;
#include <iostream>
#include <memory>
#include <functional>
#include <mutex>

#include "GlobalHelper.h"

#pragma execution_character_set("utf-8")

typedef std::function<void(uint8_t*)> VideoDataFunc;

typedef std::function<void(int, int)> VideoInfoFunc;

typedef std::function<void(int64_t)> TotalTimeFunc;

typedef std::function<void(int)> TimeFunc;

#define SLIDE_MIN_WIDTH 20      //侧边栏滑出最小的宽度
#define SLIDE_MAX_WIDTH 300     //侧边栏滑出最大的宽度
#define POS_X 20
#define POS_Y 26

#endif // _GLOBAL_H_
