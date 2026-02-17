#pragma once
#include "core/optimization/user.h"

namespace transcendental {
enum class function : uint8_t {
	exp = 0,
	ln = 1,
	sinh = 2,
	cosh = 3,
	tanh = 4,
};
};

class optimization_transcendental final : public optimization_user {
public:
	std::string import_function(transcendental::function model);
};