#pragma once
#include "synthesis/abstract.h"

class synthesis_unison : public synthesis_abstract {
protected: /* parameter */
	uint64_t depth_ = 3;
	uint64_t detune_ = 5;

public: /* constructor */
	synthesis_unison() {}
	synthesis_unison(
	    uint64_t depth,
	    uint64_t detune)
	    : depth_(depth), detune_(detune) {}

protected: /* abstract */
	virtual void synthesis(uint64_t min, uint64_t max, uint64_t period) override final;
};