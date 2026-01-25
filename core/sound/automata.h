#pragma once
#include "core/sound/abstract.h"

extern "C" {
#include <alsa/asoundlib.h>
}

class automata_singleton {
protected: /* handle */
	snd_seq_t *handle_ = nullptr;

public: /* instance */
	static automata_singleton &instance();

private: /* constraint */
	~automata_singleton();
	automata_singleton();
	automata_singleton(const automata_singleton &) = default;
	automata_singleton &operator=(const automata_singleton &) = default;
};