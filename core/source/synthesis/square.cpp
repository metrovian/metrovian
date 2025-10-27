#include "synthesis/square.h"
#include "property.h"
#include "predefined.h"

void synthesis_square::synthesis(uint64_t note, uint64_t period) {
	LOG_ENTER();
	uint64_t sample_rate = std::stoull(property_singleton::instance().parse({"synthesis", "sample-rate"}));
	std::vector<int16_t> sample(sample_rate * period, 0);
	muxer_.keysize(note);
	for (uint64_t i = 0; i < note; ++i) {
		for (uint64_t j = 0; j < sample.size(); ++j) {
			double pitch = std::pow(2.000E+0, (static_cast<int64_t>(i) - 69) / 1.200E+1) * 4.400E+2;
			double time = static_cast<double>(j) / sample_rate;
			if (std::fmod(pitch * time, 1.000E+0) < duty_) {
				sample[j] = 3276;
			} else {
				sample[j] = -3276;
			}
		}

		muxer_.keysample(sample, i);
	}

	LOG_EXIT();
	return;
}