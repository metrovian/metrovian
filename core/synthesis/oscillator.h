#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cmath>
#include <nlohmann/json.hpp>

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

public: /* oscillator lambda */
	static std::function<double(double, double)> lambda_sin();
	static std::function<double(double, double)> lambda_saw(double skew = 0.000E+0);
	static std::function<double(double, double)> lambda_square(double duty = 0.500E+0);
};