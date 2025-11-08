#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cmath>

class synthesis_oscillator {
protected: /* parameter transform */
	static double ratio(uint64_t note);
	static double frequency(uint64_t note);
	static double time(uint64_t sample);
	static double phase(uint64_t note, uint64_t sample);

public: /* oscillator */
	static double sin(uint64_t note, uint64_t sample);
	static double saw(uint64_t note, uint64_t sample, double skew = 0.000E+0);
	static double square(uint64_t note, uint64_t sample, double duty = 0.500E+0);
};