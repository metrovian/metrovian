#pragma once
#include "synthesis/abstract.h"

class synthesis_hammond : public synthesis_abstract {
protected: /* parameter */
	std::vector<double> drawbar_ = {8, 8, 8, 8, 8, 8, 8, 8, 8};

public: /* constructor */
	synthesis_hammond() {}
	synthesis_hammond(const std::vector<double> &drawbar)
	    : drawbar_(drawbar) {}

protected: /* abstract */
	virtual void synthesis(uint64_t min, uint64_t max, uint64_t period) override final;
};