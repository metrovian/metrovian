#pragma once
#include "core/sound/abstract.h"

extern "C" {
#include <alsa/asoundlib.h>
}

namespace sound {
enum class state {
	none = 0,
	attack = 1,
	decay = 2,
	sustain = 3,
	release = 4,
};

struct note {
	std::vector<int16_t> sample_;
	uint8_t active_ = 0;
	uint8_t vel_ = 0;
	uint64_t pos_enter_ = 0;
	uint64_t pos_ = 0;
	sound::state state_ = sound::state::none;
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

protected: /* envelope */
	double sustain_ = 1.000E+0;
	uint64_t attack_ = 0;
	uint64_t decay_ = 0;
	uint64_t release_ = 0;

protected: /* thread */
	void thread_event();

protected: /* envelope */
	double calc_envelope(sound::note &note);

public: /* setter */
	void set_envelope(double sustain, double attack, double decay, double release);
	void set_scale(uint16_t volume);
	void set_size(uint64_t note);
	void set_sample(uint64_t note, std::vector<int16_t> &pcm);

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