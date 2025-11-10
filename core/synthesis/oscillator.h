#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cmath>

class synthesis_oscillator {
protected: /* parameter transform */
	static double ratio(double note);
	static double frequency(double note);
	static double time(double sample);
	static double phase(double note, double sample);

public: /* oscillator */
	static double sin(double note, double sample);
	static double saw(double note, double sample, double skew = 0.000E+0);
	static double square(double note, double sample, double duty = 0.500E+0);
};