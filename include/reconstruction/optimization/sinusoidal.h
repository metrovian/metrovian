#pragma once
#include "reconstruction/optimization/abstract.h"

namespace sinusoidal {
enum class function : uint8_t {
	un_damped = 0,
	under_damped = 1,
	over_damped = 2,
	critical_damped = 3,
};
};

class optimization_sinusoidal : public optimization_abstract {
public: /* function */
	std::string import_function(sinusoidal::function model);

protected: /* abstract */
	virtual std::string objective_function() override final;
};