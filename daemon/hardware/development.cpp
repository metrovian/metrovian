#include "daemon/hardware/development.h"
#include "core/synthesis/square.h"

std::unique_ptr<indicator_abstract> hardware_development::create_indicator() {
	return std::make_unique<indicator_development>();
}

std::unique_ptr<knob_abstract> hardware_development::create_knob() {
	return std::make_unique<knob_development>();
}

std::unique_ptr<synthesis_abstract> hardware_development::read() {
	return std::make_unique<synthesis_square>();
}