#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <functional>
#include <cmath>

class synthesis_effector {
protected: /* parameter transform */
	static double note(uint64_t note, int64_t cent);

public: /* effector */
	static double pitch_shifter(
	    const std::function<double(uint64_t, uint64_t)> waveform,
	    uint64_t note,
	    uint64_t sample,
	    int64_t cent);
};