#pragma once
#include "daemon/hardware/abstract/indicator.h"

class indicator_development : public indicator_abstract {
public: /* abstract */
	virtual void set(indicator::state state) override final;
};