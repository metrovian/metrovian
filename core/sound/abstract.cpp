#include "core/sound/abstract.h"

uint16_t sound_abstract::channel() const noexcept {
	return channel_;
}

uint32_t sound_abstract::sample_rate() const noexcept {
	return sample_rate_;
}