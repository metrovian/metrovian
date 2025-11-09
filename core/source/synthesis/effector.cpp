#include "synthesis/effector.h"

double synthesis_effector::note(uint64_t note, int64_t cent) {
	return note + static_cast<double>(cent) / 1.000E+2;
}

double synthesis_effector::pitch_shifter(
    const std::function<double(uint64_t, uint64_t)> waveform,
    uint64_t note,
    uint64_t sample,
    int64_t cent) {
	double ratio = std::pow(2.000E+0, static_cast<double>(cent) / 1.200E+3);
	double pos = static_cast<double>(sample) * ratio;
	double frac = pos - std::floor(pos);
	double sample1 = waveform(note, std::floor(pos));
	double sample2 = waveform(note, std::ceil(pos));
	return sample2 * frac + sample1 * (1.000E+0 - frac);
}
