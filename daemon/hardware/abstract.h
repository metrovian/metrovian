#pragma once
#include "daemon/hardware/abstract/indicator.h"
#include "daemon/hardware/abstract/knob.h"
#include "daemon/state/abstract.h"
#include "core/synthesis/abstract.h"

class hardware_abstract {
protected: /* hardware component */
	std::unique_ptr<indicator_abstract> indicator_ = nullptr;
	std::unique_ptr<knob_abstract> knob_ = nullptr;

public: /* transition */
	void enter(machine::state state);
	void exit(machine::state state);
	void create();

public: /* destructor */
	virtual ~hardware_abstract() {}

protected: /* abstract */
	virtual std::unique_ptr<indicator_abstract> create_indicator() = 0;
	virtual std::unique_ptr<knob_abstract> create_knob() = 0;

public: /* abstract */
	virtual std::unique_ptr<synthesis_abstract> update() = 0;
};