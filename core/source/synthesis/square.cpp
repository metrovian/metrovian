#include "synthesis/square.h"
#include "property.h"
#include "predefined.h"

void synthesis_square::synthesis(uint64_t min, uint64_t max, uint64_t period) {
	LOG_ENTER();
	uint64_t sample_rate = CONFIG_UINT64("synthesis", "sample-rate");
	std::vector<int16_t> sample(sample_rate * period, 0);
	resize(max);
	for (uint64_t i = min; i < max; ++i) {
		for (uint64_t j = 0; j < sample.size(); ++j) {
			sample[j] = synthesis_oscillator::square(i, j, duty_) * 3276;
		}

		resample(i, sample);
	}

	LOG_EXIT();
	return;
}