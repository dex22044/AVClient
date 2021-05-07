#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
}
#include <iostream>
#include "MsgBoxes.h"

class MediaDecoder
{
public:
	AVCodecContext* aCodecCtx;
	AVCodecContext* vCodecCtx;

	MediaDecoder(int width, int height);
	AVFrame* DecodeVideoFrame(AVPacket pkt);
};

