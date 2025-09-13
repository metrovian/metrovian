#pragma once
#include "optimization/abstract.h"

namespace oscillator {
enum class motion : uint8_t {
	under_damped = 0,
	over_damped = 1,
	critical_damped = 2,
};
};

class optimization_oscillator : public optimization_abstract {
public: /* motion */
	std::string import_function(oscillator::motion model);

protected: /* abstract */
	virtual std::string objective_function() override final;
};