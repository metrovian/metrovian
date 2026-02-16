#pragma once
#include "core/sound/abstract.h"

extern "C" {
#include <alsa/asoundlib.h>
}

class sound_player final : public sound_consumer {
public:
	sound_player(
	    uint16_t channel,
	    uint32_t sample_rate)
	    : sound_consumer(channel, sample_rate) {}

	int open() override;
	int close() override;
	void consume(std::vector<int16_t> &pcm) override;

protected:
	snd_pcm_t *handle_ = nullptr;
};