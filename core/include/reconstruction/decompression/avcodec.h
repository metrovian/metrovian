#pragma once
#include "reconstruction/decompression/audio.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
#include <libavutil/mem.h>
#include <libswresample/swresample.h>
}

class decompression_avcodec : public decompression_audio {
protected: /* context */
	AVCodec *avcodec_ = nullptr;
	AVFormatContext *avformat_ctx_ = nullptr;
	AVCodecContext *avcodec_ctx_ = nullptr;
	AVCodecParameters *avcodec_params_ = nullptr;

protected: /* abstract */
	virtual int8_t open(const std::string &path) override final;
	virtual int8_t close() override final;
	virtual std::vector<uint8_t> decompression(const std::vector<uint8_t> &payload) override final;
};