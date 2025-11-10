#include "core/synthesis/saw.h"
#include "core/property.h"
#include "core/predefined.h"

void synthesis_saw::synthesis(uint64_t min, uint64_t max, uint64_t period) {
	LOG_ENTER();
	uint64_t sample_rate = CONFIG_UINT64("synthesis", "sample-rate");
	std::vector<int16_t> sample(sample_rate * period, 0);
	resize(max);
	for (uint64_t i = min; i < max; ++i) {
		for (uint64_t j = 0; j < sample.size(); ++j) {
			sample[j] = synthesis_oscillator::saw(i, j, skew_) * 3276;
		}

		resample(i, sample);
		LOG_PROGRESS((i + 1) - min, max - min);
	}

	LOG_EXIT();
	return;
}