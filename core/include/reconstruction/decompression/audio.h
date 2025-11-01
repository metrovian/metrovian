#pragma once
#include "reconstruction/decompression/abstract.h"
#include "sound/abstract.h"
#include "sound/factory.h"
#include "sound/player.h"
#include "sound/writer.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
#include <libavutil/mem.h>
#include <libswresample/swresample.h>
}

class decompression_producer : public sound_producer {
protected: /* context */
	AVCodec *avcodec_ = nullptr;
	AVFormatContext *avformat_ctx_ = nullptr;
	AVCodecContext *avcodec_ctx_ = nullptr;
	AVCodecParameters *avcodec_params_ = nullptr;

protected: /* paramter */
	int32_t stream_ = 0;

public: /* property */
	uint16_t channel();
	uint32_t sample_rate();

public: /* setter */
	void seturi(const std::string &path);

public: /* constructor */
	decompression_producer() : sound_producer(0, 0) {}

public: /* abstract */
	virtual int8_t open() override final;
	virtual int8_t close() override final;

public: /* abstract */
	virtual std::vector<int16_t> produce() override final;
};

class decompression_audio : public decompression_abstract, public sound_factory {
protected: /* property */
	uint16_t channel();
	uint32_t sample_rate();

protected: /* setter */
	void prepare(const std::string &path);

public: /* overload */
	virtual void decompress(const std::string &path) override final;
	virtual void decompress(const std::string &path, const std::string &record) override final;

protected: /* abstract */
	virtual std::unique_ptr<sound_producer> create_producer() override final;
	virtual std::unique_ptr<sound_consumer> create_consumer() override final;
};