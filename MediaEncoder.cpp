#include "MediaEncoder.h"

MediaEncoder::MediaEncoder(int width, int height) {
	//AVCodec* aCodec = avcodec_find_encoder(AV_CODEC_ID_AAC);
	AVCodec* vCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if(vCodec == NULL) { ErrorMessageBox("Failed to get X264 encoder."); exit(-1); }
	vCodecCtx = avcodec_alloc_context3(vCodec);
    vCodecCtx->sample_fmt = AV_SAMPLE_FMT_S16;
    vCodecCtx->bit_rate = 1 * 1000000; // 1Mbps
    vCodecCtx->time_base = { 1, 30 };
    vCodecCtx->width = width;
    vCodecCtx->height = height;
    vCodecCtx->pix_fmt = AV_PIX_FMT_YUV444P;
    vCodecCtx->gop_size = 100;
    vCodecCtx->level = 41;
    if (av_opt_set(vCodecCtx->priv_data, "profile", "high444", 0) < 0) { ErrorMessageBox("Failed to set X264 profile."); exit(-1); };
    if (av_opt_set(vCodecCtx->priv_data, "preset", "ultrafast", 0) < 0) { ErrorMessageBox("Failed to set X264 preset."); exit(-1); };
    if (av_opt_set(vCodecCtx->priv_data, "tune", "zerolatency", 0) < 0) { ErrorMessageBox("Failed to set X264 tune."); exit(-1); };
    if (avcodec_open2(vCodecCtx, vCodec, NULL) < 0) { ErrorMessageBox("Failed to open X264 stream."); exit(-1); };
}

AVPacket* MediaEncoder::EncodeVideoFrame(AVFrame* frame) {
    AVPacket pkt = { 0 };
    av_init_packet(&pkt);
    pkt.pts = pts++;
    frame->pts = pkt.pts;
    int got_output = 0;
    if (avcodec_encode_video2(vCodecCtx, &pkt, frame, &got_output) < 0) {
        return NULL;
    }
    else if (got_output == 0) {
        return NULL;
    }
    else {
        return &pkt;
    }
}