#pragma once
#include "core/optimization/abstract.h"

class optimization_polynomial final : public optimization_abstract {
public:
	std::string import_function(uint8_t model);

protected:
	std::string objective_function() override;
};