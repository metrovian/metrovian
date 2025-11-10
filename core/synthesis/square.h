#pragma once
#include "core/synthesis/abstract.h"

class synthesis_square : public synthesis_abstract {
protected: /* parameter */
	double duty_ = 5.000E-1;

public: /* constructor */
	synthesis_square() {}
	synthesis_square(double duty) : duty_(duty) {}

protected: /* abstract */
	virtual void synthesis(uint64_t min, uint64_t max, uint64_t period) override final;
};