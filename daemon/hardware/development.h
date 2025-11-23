#pragma once
#include "daemon/hardware/abstract.h"
#include "daemon/hardware/development/indicator.h"
#include "daemon/hardware/development/knob.h"

class hardware_development : public hardware_abstract {
protected: /* abstract */
	virtual std::unique_ptr<indicator_abstract> create_indicator() override final;
	virtual std::unique_ptr<knob_abstract> create_knob() override final;

public: /* abstract */
	virtual std::unique_ptr<synthesis_abstract> read() override final;
};