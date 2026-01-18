#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cmath>
#include <nlohmann/json.hpp>

typedef std::unordered_map<
    std::string,
    std::function<std::function<double(double, double)>(const nlohmann::ordered_json &)>>
    oscmap_t;

class synthesis_oscillator {
protected: /* parameter transform */
	static double ratio(double note);
	static double frequency(double note);
	static double time(double sample);
	static double phase(double note, double sample);

protected: /* oscillator */
	static double sin(double note, double sample);
	static double saw(double note, double sample, double skew = 0.000E+0);
	static double sqr(double note, double sample, double duty = 0.500E+0);

protected: /* oscillator lambda */
	static std::function<double(double, double)> lambda_sin();
	static std::function<double(double, double)> lambda_saw(double skew = 0.000E+0);
	static std::function<double(double, double)> lambda_sqr(double duty = 0.500E+0);

public: /* oscillator map */
	static const oscmap_t &map();
};