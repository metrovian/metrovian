#pragma once
#include "reconstruction/optimization/abstract.h"

class optimization_polynomial : public optimization_abstract {
public: /* function */
	std::string import_function(uint8_t model);

protected: /* abstract */
	virtual std::string objective_function() override final;
};