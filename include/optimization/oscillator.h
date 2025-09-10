#pragma once
#include "optimization/abstract.h"

namespace oscillator {
enum class motion : uint8_t {
	harmonic = 0,
	parametric = 1,
};
};

class optimization_oscillator : public optimization_abstract {
public: /* motion */
	std::string import_function(oscillator::motion model);

protected: /* abstract */
	virtual std::string objective_function() override final;
};