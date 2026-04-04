#pragma once
#include "core/sound/abstract.h"

#include <atomic>
#include <functional>

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
	uint8_t active_ = 0;
	uint8_t vel_ = 0;
	uint64_t key_ = 0;
	uint64_t pos_enter_ = 0;
	uint64_t pos_ = 0;
	double env_ = 0;
	sound::state state_ = sound::state::none;
};
}; // namespace sound

class sound_sequencer final : public sound_producer {
public:
	sound_sequencer(
	    uint16_t channel,
	    uint32_t sample_rate)
	    : sound_producer(channel, sample_rate) {}

	void set_envelope(double sustain, double attack, double decay, double release);
	void set_scale(uint16_t volume);
	void set_size(uint64_t note);
	void callback_disconnect(std::function<void(void)> function);
	void callback_change(std::function<void(uint32_t, int32_t)> function);
	void callback_synthesis(std::function<int16_t(uint64_t, uint64_t)> function);
	int open() override;
	int close() override;
	std::vector<int16_t> produce() override;

private:
	double calc_envelope(sound::note &note);

private:
	snd_seq_t *handle_ = nullptr;
	std::function<void(void)> on_disconnect_ = nullptr;
	std::function<void(uint32_t, int32_t)> on_change_ = nullptr;
	std::function<int16_t(uint64_t, uint64_t)> on_synthesis_ = nullptr;
	std::atomic<uint8_t> state_;
	std::mutex mutex_;
	std::vector<sound::note> key_;
	uint64_t len_ = 0;
	uint16_t volume_ = 64;
	double sustain_ = 1.000E+0;
	uint64_t attack_ = 0;
	uint64_t decay_ = 0;
	uint64_t release_ = 0;
};
