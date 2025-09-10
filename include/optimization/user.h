#pragma once
#include "optimization/abstract.h"

class optimization_user : public optimization_abstract {
protected: /* abstract */
	virtual std::string objective_function() override final;
};
