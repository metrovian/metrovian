#pragma once
#include "core/optimization/user.h"

class optimization_polynomial final : public optimization_user {
public:
	std::string import_function(uint8_t model);
};