#pragma once
#include "core/optimization/user.h"

namespace sinusoidal {
enum class function : uint8_t {
	un_damped = 0,
	under_damped = 1,
	over_damped = 2,
	critical_damped = 3,
};
};

class optimization_sinusoidal final : public optimization_user {
public:
	std::string import_function(sinusoidal::function model);
};