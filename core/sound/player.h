#pragma once
#include "core/sound/abstract.h"

extern "C" {
#include <alsa/asoundlib.h>
}

class sound_player : public sound_consumer {
protected: /* handle */
	snd_pcm_t *handle_ = nullptr;

public: /* constructor */
	sound_player(
	    uint16_t channel,
	    uint32_t sample_rate)
	    : sound_consumer(channel, sample_rate) {}

public: /* abstract */
	virtual int8_t open() override final;
	virtual int8_t close() override final;

public: /* abstract */
	virtual void consume(std::vector<int16_t> &pcm) override final;
};