#pragma once
#include "core/reconstruction/optimization/abstract.h"

namespace transcendental {
enum class function : uint8_t {
	exp = 0,
	ln = 1,
	sinh = 2,
	cosh = 3,
	tanh = 4,
};
};

class optimization_transcendental : public optimization_abstract {
public: /* function */
	std::string import_function(transcendental::function model);

protected: /* abstract */
	virtual std::string objective_function() override final;
};