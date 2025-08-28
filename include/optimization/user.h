#pragma once
#include "optimization/abstract.h"

class optimization_user : public optimization_abstract {
protected: /* custom function */
	std::string function_;

public: /* custom function */
	std::string export_function();
	std::string import_function(const std::string &function);

protected: /* abstract */
	virtual std::string objective_function() override final;
};
