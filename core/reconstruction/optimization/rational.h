#pragma once
#include "core/reconstruction/optimization/abstract.h"

class optimization_rational : public optimization_abstract {
public: /* function */
	std::string import_function(std::vector<uint8_t> model);

protected: /* abstract */
	virtual std::string objective_function() override final;
};