#pragma once
#include "reconstruction/decompression/abstract.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
#include <libavutil/mem.h>
#include <libswresample/swresample.h>
#include <alsa/asoundlib.h>
}

class decompression_audio : public decompression_abstract {
protected: /* parameter */
	uint16_t channels_ = 0;
	uint32_t sample_rate_ = 0;

public: /* overload */
	virtual void decompress(const std::string &path) override final;
	virtual void decompress(const std::string &path, const std::string &record) override final;
};