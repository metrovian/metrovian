#pragma once
#include "core/synthesis/abstract.h"

class synthesis_add : public synthesis_abstract {
protected: /* synthesis components */
	std::vector<std::function<double(double, double)>> components_;
	std::vector<double> amps_;
	std::vector<double> ratios_;

public: /* constructor */
	synthesis_add(nlohmann::json preset);

protected: /* abstract */
	virtual void synthesis(uint64_t min, uint64_t max, uint64_t period) override final;
};