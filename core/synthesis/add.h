#pragma once
#include "core/synthesis/abstract.h"

class synthesis_add final : public synthesis_abstract {
public:
	synthesis_add(const nlohmann::ordered_json &preset);

protected:
	void synthesis(uint64_t min, uint64_t max, uint64_t period) override;

protected:
	std::vector<std::function<double(double, double)>> components_;
	std::vector<double> amps_;
	std::vector<double> ratios_;
};