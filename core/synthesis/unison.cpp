#include "core/synthesis/unison.h"
#include "core/synthesis/effector.h"
#include "core/property.h"
#include "core/predefined.h"

void synthesis_unison::synthesis(uint64_t min, uint64_t max, uint64_t period) {
	LOG_ENTER();
	std::vector<double> cent(depth_, 0.000E+0);
	double start = depth_;
	start *= detune_;
	start /= -2.000E+0;
	for (uint64_t i = 0; i < depth_; ++i) {
		cent[i] = start + detune_ * static_cast<double>(i);
		cent[i] /= 1.000E+2;
	}

	uint64_t sample_rate = CONFIG_UINT64("synthesis", "sample-rate");
	std::vector<int16_t> sample(sample_rate * period, 0);
	resize(max);
	for (uint64_t i = min; i < max; ++i) {
		for (uint64_t j = 0; j < sample.size(); ++j) {
			double sum = 0.000E+0;
			for (uint64_t k = 0; k < depth_; ++k) {
				sum += synthesis_oscillator::saw(i + cent[k], j);
			}

			sample[j] = (sum / std::sqrt(depth_)) * 3276;
		}

		resample(i, sample);
		LOG_PROGRESS((i + 1) - min, max - min);
	}

	LOG_EXIT();
	return;
}