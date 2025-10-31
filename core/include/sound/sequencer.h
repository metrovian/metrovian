#pragma once
#include "sound/abstract.h"

extern "C" {
#include <alsa/asoundlib.h>
}

namespace sound {
struct note {
	std::vector<int16_t> sample_;
	uint8_t active_ = 0;
	uint8_t vel_ = 0;
	uint64_t pos_ = 0;
};
}; // namespace sound

class sound_sequencer : public sound_producer {
protected: /* handle */
	snd_seq_t *handle_ = nullptr;

protected: /* control */
	std::atomic<uint8_t> state_;
	std::mutex mutex_;
	std::vector<sound::note> key_;

protected: /* parameter */
	uint64_t len_ = 0;

protected: /* thread */
	void thread_event();

public: /* setter */
	void resize(uint64_t note);
	void resample(uint64_t note, std::vector<int16_t> &pcm);

public: /* constructor */
	sound_sequencer(
	    uint16_t channel,
	    uint32_t sample_rate)
	    : sound_producer(channel, sample_rate) {}

public: /* abstract */
	virtual int8_t open() override final;
	virtual int8_t close() override final;

public: /* abstract */
	virtual std::vector<int16_t> produce() override final;
};