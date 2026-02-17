#pragma once
#include "core/optimization/user.h"

class optimization_rational final : public optimization_user {
public:
	std::string import_function(std::vector<uint8_t> model);
};