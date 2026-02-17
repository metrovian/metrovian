#pragma once
#include "core/optimization/abstract.h"

class optimization_rational final : public optimization_abstract {
public:
	std::string import_function(std::vector<uint8_t> model);

protected:
	std::string objective_function() override;
};