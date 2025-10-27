#include "synthesis/saw.h"
#include "property.h"
#include "predefined.h"

void synthesis_saw::synthesis(uint64_t note, uint64_t period) {
	LOG_ENTER();
	uint64_t sample_rate = std::stoull(property_singleton::instance().parse({"synthesis", "sample-rate"}));
	std::vector<int16_t> sample(sample_rate * period, 0);
	muxer_.keysize(note);
	for (uint64_t i = 0; i < note; ++i) {
		for (uint64_t j = 0; j < sample.size(); ++j) {
			double time = static_cast<double>(j) / sample_rate;
			double pitch = std::pow(2.000E+0, (static_cast<int64_t>(i) - 69) / 1.200E+1) * 4.400E+2;
			double phase = std::fmod(pitch * time, 1.000E+0);
			if (phase < skew_) {
				sample[j] = ((phase / skew_) * 2.000E+0 - 1.000E+0) * 3276;
			} else {
				sample[j] = ((1.000E+0 - (phase - skew_) / (1.000E+0 - skew_)) * 2.000E+0 - 1.000E+0) * 3276;
			}
		}

		muxer_.keysample(sample, i);
	}

	LOG_EXIT();
	return;
}