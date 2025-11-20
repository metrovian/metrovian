#pragma once
#include "daemon/hardware/abstract/indicator.h"
#include "daemon/hardware/abstract/knob.h"

class hardware_abstract {
protected: /* hardware component */
	std::unique_ptr<indicator_abstract> power_ = nullptr;
	std::unique_ptr<indicator_abstract> status_ = nullptr;
	std::unique_ptr<knob_abstract> knob_ = nullptr;
};