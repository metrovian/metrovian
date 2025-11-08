#pragma once
#include "synthesis/abstract.h"

class synthesis_sin : public synthesis_abstract {
protected: /* abstract */
	virtual void synthesis(uint64_t min, uint64_t max, uint64_t period) override final;
};