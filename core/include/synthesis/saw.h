#include "synthesis/abstract.h"

class synthesis_saw : public synthesis_abstract {
protected: /* parameter */
	double skew_ = 0.000E+0;

public: /* constructor */
	synthesis_saw() {}
	synthesis_saw(double skew) : skew_(skew) {}

protected: /* abstract */
	virtual void synthesis(uint64_t min, uint64_t max, uint64_t period) override final;
};