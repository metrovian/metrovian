#pragma once
#include "synthesis/abstract.h"

class synthesis_unison : public synthesis_abstract {
protected: /* parameter */
	double depth_ = 3.000E+0;
	double detune_ = 5.000E+0;

public: /* constructor */
	synthesis_unison() {}
	synthesis_unison(
	    double depth,
	    double detune)
	    : depth_(depth), detune_(detune) {}

protected: /* abstract */
	virtual void synthesis(uint64_t min, uint64_t max, uint64_t period) override final;
};