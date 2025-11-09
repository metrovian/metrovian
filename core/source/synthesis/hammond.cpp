#include "synthesis/hammond.h"
#include "property.h"
#include "predefined.h"

void synthesis_hammond::synthesis(uint64_t min, uint64_t max, uint64_t period) {
	LOG_ENTER();
	std::vector<double> note(9, 0.000E+0);
	note[0] = std::log2(5.000E-1);
	note[1] = std::log2(1.500E+0);
	note[2] = std::log2(1.000E+0);
	note[3] = std::log2(2.000E+0);
	note[4] = std::log2(3.000E+0);
	note[5] = std::log2(4.000E+0);
	note[6] = std::log2(5.000E+0);
	note[7] = std::log2(6.000E+0);
	note[8] = std::log2(8.000E+0);
	for (uint64_t i = 0; i < 9; ++i) {
		note[i] *= 1.200E+1;
	}

	uint64_t sample_rate = CONFIG_UINT64("synthesis", "sample-rate");
	std::vector<int16_t> sample(sample_rate * period, 0);
	resize(max);
	for (uint64_t i = min; i < max; ++i) {
		for (uint64_t j = 0; j < sample.size(); ++j) {
			double sum = 0.000E+0;
			for (uint64_t k = 0; k < 9; ++k) {
				sum += synthesis_oscillator::sin(i + note[k], j) * drawbar_[k];
			}

			sample[j] = (sum / 2.400E+1) * 3276;
		}

		resample(i, sample);
		LOG_PROGRESS((i + 1) - min, max - min);
	}

	LOG_EXIT();
	return;
}