#include "synthesis/oscillator.h"
#include "property.h"
#include "predefined.h"

double synthesis_oscillator::ratio(uint64_t note) {
	return std::pow(2.000E+0, (static_cast<int64_t>(note) - 69) / 1.200E+1);
}

double synthesis_oscillator::frequency(uint64_t note) {
	static const double tune = CONFIG_FLOAT64("synthesis", "tune");
	return ratio(note) * tune;
}

double synthesis_oscillator::time(uint64_t sample) {
	static const double sample_rate = CONFIG_FLOAT64("synthesis", "sample-rate");
	return static_cast<double>(sample) / sample_rate;
}

double synthesis_oscillator::phase(uint64_t note, uint64_t sample) {
	return std::fmod(frequency(note) * time(sample), 1.000E+0);
}

double synthesis_oscillator::sin(uint64_t note, uint64_t sample) {
	return std::sin(phase(note, sample) * (M_PI * 2.000E+0));
}

double synthesis_oscillator::saw(uint64_t note, uint64_t sample, double skew) {
	double result = 0.000E+0;
	if (phase(note, sample) < skew) {
		result = phase(note, sample) / skew;
		result *= 2.000E+0;
		result -= 1.000E+0;
	} else {
		result = phase(note, sample) - skew;
		result /= (skew - 1.000E+0);
		result += 1.000E+0;
		result *= 2.000E+0;
		result -= 1.000E+0;
	}

	return result;
}

double synthesis_oscillator::square(uint64_t note, uint64_t sample, double duty) {
	return phase(note, sample) < duty ? 1.000E+0 : -1.000E+0;
}
