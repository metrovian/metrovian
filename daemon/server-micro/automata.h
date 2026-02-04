#pragma once
#include "core/sound/abstract.h"

extern "C" {
#include <alsa/asoundlib.h>
#include <smf.h>
}

class automata_singleton {
protected: /* handle */
	snd_seq_t *handle_ = nullptr;
	smf_t *smf_ = nullptr;

protected: /* control */
	std::atomic<uint8_t> state_ = 0;
	int port_ = 0;
	const int noteon_ = 0x90;
	const int noteoff_ = 0x80;

protected: /* thread */
	void thread_playback();

public: /* getter */
	uint8_t get_state();

public: /* export */
	int8_t open(std::string name);
	int8_t close();

public: /* instance */
	static automata_singleton &instance();

private: /* constraint */
	~automata_singleton();
	automata_singleton();
	automata_singleton(const automata_singleton &) = default;
	automata_singleton &operator=(const automata_singleton &) = default;
};