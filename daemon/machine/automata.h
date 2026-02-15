#pragma once
#include "core/sound/abstract.h"

extern "C" {
#include <alsa/asoundlib.h>
#include <smf.h>
}

class automata_singleton {
public:
	static automata_singleton &instance();

public:
	automata_singleton(const automata_singleton &) = delete;
	automata_singleton(automata_singleton &&) = delete;
	automata_singleton &operator=(const automata_singleton &) = delete;
	automata_singleton &operator=(automata_singleton &&) = delete;
	int8_t play(std::string name);
	int8_t panic();

private:
	~automata_singleton();
	automata_singleton();

private:
	snd_seq_t *handle_ = nullptr;
	smf_t *smf_ = nullptr;
	std::atomic<uint8_t> state_ = 0;
	int port_ = 0;
	const int noteon_ = 0x90;
	const int noteoff_ = 0x80;
};