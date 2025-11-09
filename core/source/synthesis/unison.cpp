#include "synthesis/unison.h"
#include "synthesis/effector.h"
#include "property.h"
#include "predefined.h"

void synthesis_unison::synthesis(uint64_t min, uint64_t max, uint64_t period) {
	LOG_ENTER();
	uint64_t sample_rate = CONFIG_UINT64("synthesis", "sample-rate");
	std::vector<int16_t> sample(sample_rate * period, 0);
	std::vector<int64_t> cent(depth_, 0);
	resize(max);
	auto waveform = [&](uint64_t note, uint64_t sample) {
		return synthesis_oscillator::saw(note, sample);
	};

	int64_t start = depth_;
	start *= detune_;
	start /= -2;
	for (uint64_t i = 0; i < depth_; ++i) {
		cent[i] = start + detune_ * static_cast<int64_t>(i);
	}

	for (uint64_t i = min; i < max; ++i) {
		for (uint64_t j = 0; j < sample.size(); ++j) {
			double sum = 0.000E+0;
			for (uint64_t k = 0; k < depth_; ++k) {
				sum += synthesis_effector::shift(waveform, i, j, cent[k]);
			}

			sample[j] = (sum / std::sqrt(depth_)) * 3276;
		}

		resample(i, sample);
	}

	LOG_EXIT();
	return;
}