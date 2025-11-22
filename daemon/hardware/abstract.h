#pragma once
#include "daemon/hardware/abstract/indicator.h"
#include "daemon/hardware/abstract/knob.h"
#include "daemon/hardware/abstract/segment.h"
#include "daemon/state/abstract.h"

class hardware_abstract {
protected: /* hardware component */
	std::unique_ptr<indicator_abstract> indicator_ = nullptr;
	std::unique_ptr<knob_abstract> knob_ = nullptr;
	std::unique_ptr<segment_abstract> segment_ = nullptr;

public: /* export */
	void enter(machine::state state);
	void exit(machine::state state);

protected: /* abstract */
	virtual void create_indicator() = 0;
	virtual void create_knob() = 0;
	virtual void create_segment() = 0;
};