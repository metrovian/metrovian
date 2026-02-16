#pragma once
#include "core/sound/abstract.h"

#include <string>
#include <fstream>

class sound_writer final : public sound_consumer {
public:
	sound_writer(
	    const std::string &path,
	    uint16_t channel,
	    uint32_t sample_rate)
	    : sound_consumer(channel, sample_rate), path_(path) {}

	int open() override;
	int close() override;
	void consume(std::vector<int16_t> &pcm) override;

private:
	std::ofstream output_;
	std::string path_;
	uint32_t chunk_ = 36;
	uint32_t data_ = 0;
};