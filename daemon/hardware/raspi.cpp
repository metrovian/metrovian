#include "daemon/hardware/raspi.h"
#include "core/synthesis/square.h"

std::unique_ptr<indicator_abstract> hardware_raspi::create_indicator() {
	return std::make_unique<indicator_raspi>();
}

std::unique_ptr<knob_abstract> hardware_raspi::create_knob() {
	return std::make_unique<knob_raspi>();
}

std::unique_ptr<synthesis_abstract> hardware_raspi::update() {
	if (knob_->update() != 0) {
		return std::make_unique<synthesis_square>();
	}

	return nullptr;
}