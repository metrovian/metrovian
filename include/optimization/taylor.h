#pragma once
#include "optimization/abstract.h"

class optimization_taylor : public optimization_abstract {
public: /* function */
	std::string import_function(uint8_t model);

protected: /* abstract */
	virtual std::string objective_function() override final;
};