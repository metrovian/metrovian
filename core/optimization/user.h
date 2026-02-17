#pragma once
#include "core/optimization/abstract.h"

class optimization_user : public optimization_abstract {
protected:
	std::string objective_function() override;
};
