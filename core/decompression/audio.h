#pragma once
#include "core/decompression/abstract.h"
#include "core/sound/factory.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
#include <libavutil/mem.h>
#include <libswresample/swresample.h>
}

class decompression_producer final : public sound_producer {
public:
	decompression_producer() : sound_producer(0, 0) {}
	void prepare(const std::string &path);
	int open() override;
	int close() override;
	std::vector<int16_t> produce() override;

private:
	AVCodec *avcodec_ = nullptr;
	AVFormatContext *avformat_ctx_ = nullptr;
	AVCodecContext *avcodec_ctx_ = nullptr;
	AVCodecParameters *avcodec_params_ = nullptr;
	int stream_ = 0;
};

class decompression_audio final : public decompression_abstract, public sound_factory {
public:
	void decompress(const std::string &path) override;
	void decompress(const std::string &path, const std::string &record) override;

private:
	void prepare(const std::string &path);

private:
	std::unique_ptr<sound_producer> create_producer() override;
	std::unique_ptr<sound_consumer> create_consumer() override;
};