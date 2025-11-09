#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <functional>
#include <cmath>

class synthesis_effector {
public: /* effector */
	static double shift(
	    const std::function<double(uint64_t, uint64_t)> waveform,
	    double cent,
	    uint64_t note,
	    uint64_t sample);
};