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

class MediaEncoder
{
private:
	int pts;

public:
	AVCodecContext* aCodecCtx;
	AVCodecContext* vCodecCtx;

	MediaEncoder(int width, int height);
	AVPacket* EncodeVideoFrame(AVFrame* frame);
};

