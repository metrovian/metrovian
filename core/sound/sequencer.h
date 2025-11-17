#pragma once
#include "core/sound/abstract.h"

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

protected: /* callback */
	std::function<void(void)> on_disconnect_ = nullptr;
	std::function<void(unsigned, int)> on_change_ = nullptr;

protected: /* control */
	std::atomic<uint8_t> state_;
	std::mutex mutex_;
	std::vector<sound::note> key_;

protected: /* parameter */
	uint64_t len_ = 0;
	uint16_t volume_ = 64;

protected: /* thread */
	void thread_event();

public: /* setter */
	void rescale(uint16_t volume);
	void resize(uint64_t note);
	void resample(uint64_t note, std::vector<int16_t> &pcm);

public: /* callback */
	void callback_disconnect(std::function<void(void)> function);
	void callback_change(std::function<void(unsigned, int)> function);

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