#pragma once
#include "core/optimization/abstract.h"

namespace sinusoidal {
enum class function : uint8_t {
	un_damped = 0,
	under_damped = 1,
	over_damped = 2,
	critical_damped = 3,
};
};

class optimization_sinusoidal final : public optimization_abstract {
public:
	std::string import_function(sinusoidal::function model);

protected:
	std::string objective_function() override;
};