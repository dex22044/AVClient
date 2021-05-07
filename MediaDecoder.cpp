#include "MediaDecoder.h"

MediaDecoder::MediaDecoder(int width, int height)
{
    AVCodec* vCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (vCodec == NULL) { ErrorMessageBox("Failed to get X264 decoder."); exit(-1); }
    vCodecCtx = avcodec_alloc_context3(vCodec);
    vCodecCtx->time_base = { 1, 30 };
    vCodecCtx->width = width;
    vCodecCtx->height = height;
    vCodecCtx->pix_fmt = AV_PIX_FMT_YUV444P;
    if (avcodec_open2(vCodecCtx, vCodec, NULL) < 0) { ErrorMessageBox("Failed to open X264 stream."); exit(-1); };
}

AVFrame* MediaDecoder::DecodeVideoFrame(AVPacket pkt)
{
    AVFrame* frame = av_frame_alloc();
    int got_frame = 0;
    avcodec_decode_video2(vCodecCtx, frame, &got_frame, &pkt);
    if (got_frame == 0) {
        av_frame_free(&frame);
        return NULL;
    }
    else return frame;
}
