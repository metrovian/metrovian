#pragma once
#include "core/sound/abstract.h"

class sound_writer : public sound_consumer {
protected: /* output */
	std::ofstream output_;
	std::string path_;

protected: /* parameter */
	uint32_t chunk_ = 36;
	uint32_t data_ = 0;

public: /* constructor */
	sound_writer(
	    const std::string &path,
	    uint16_t channel,
	    uint32_t sample_rate)
	    : sound_consumer(channel, sample_rate), path_(path) {}

public: /* abstract */
	virtual int8_t open() override final;
	virtual int8_t close() override final;

public: /* abstract */
	virtual void consume(std::vector<int16_t> &pcm) override final;
};