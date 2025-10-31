#include "synthesis/sin.h"
#include "property.h"
#include "predefined.h"

void synthesis_sin::synthesis(uint64_t note, uint64_t period) {
	LOG_ENTER();
	uint64_t sample_rate = CONFIG_UINT64("synthesis", "sample-rate");
	std::vector<int16_t> sample(sample_rate * period, 0);
	resize(note);
	for (uint64_t i = 0; i < note; ++i) {
		for (uint64_t j = 0; j < sample.size(); ++j) {
			double time = static_cast<double>(j) / sample_rate;
			double pitch = std::pow(2.000E+0, (static_cast<int64_t>(i) - 69) / 1.200E+1) * 4.400E+2;
			double phase = std::fmod(pitch * time, 1.000E+0);
			sample[j] = static_cast<int16_t>(std::sin(phase * (M_PI * 2.000E+0)) * 3276);
		}

		resample(i, sample);
	}

	LOG_EXIT();
	return;
}