#include "core/synthesis/oscillator.h"
#include "core/property.h"
#include "core/predefined.h"

const oscmap_t &synthesis_oscillator::map() {
	static const oscmap_t map_ =
	    {{"sin", [](const nlohmann::ordered_json &) { return lambda_sin(); }},
	     {"saw", [](const nlohmann::ordered_json &obj) { return lambda_saw(obj.value("skew", 0.000E+0)); }},
	     {"sqr", [](const nlohmann::ordered_json &obj) { return lambda_sqr(obj.value("duty", 0.500E+0)); }}};

	return map_;
}

double synthesis_oscillator::ratio(double note) {
	return std::pow(2.000E+0, (note - 6.900E+1) / 1.200E+1);
}

double synthesis_oscillator::frequency(double note) {
	static const double tune = CONFIG_FLOAT64("synthesis", "tune");
	return ratio(note) * tune;
}

double synthesis_oscillator::time(double sample) {
	static const double sample_rate = CONFIG_FLOAT64("synthesis", "sample-rate");
	return sample / sample_rate;
}

double synthesis_oscillator::phase(double note, double sample) {
	return std::fmod(frequency(note) * time(sample), 1.000E+0);
}

double synthesis_oscillator::sin(double note, double sample) {
	return std::sin(phase(note, sample) * (M_PI * 2.000E+0));
}

double synthesis_oscillator::saw(double note, double sample, double skew) {
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

double synthesis_oscillator::sqr(double note, double sample, double duty) {
	return phase(note, sample) < duty ? 1.000E+0 : -1.000E+0;
}

std::function<double(double, double)> synthesis_oscillator::lambda_sin() {
	return [](double note, double sample) {
		return sin(note, sample);
	};
}

std::function<double(double, double)> synthesis_oscillator::lambda_saw(double skew) {
	return [skew](double note, double sample) {
		return saw(note, sample, skew);
	};
}

std::function<double(double, double)> synthesis_oscillator::lambda_sqr(double duty) {
	return [duty](double note, double sample) {
		return sqr(note, sample, duty);
	};
}