#include "daemon/hardware/raspi.h"
#include "core/synthesis/sin.h"
#include "core/synthesis/saw.h"
#include "core/synthesis/square.h"
#include "core/synthesis/unison.h"
#include "core/synthesis/hammond.h"

std::unique_ptr<indicator_abstract> hardware_raspi::create_indicator() {
	return std::make_unique<indicator_raspi>();
}

std::unique_ptr<knob_abstract> hardware_raspi::create_knob() {
	return std::make_unique<knob_raspi>();
}

std::unique_ptr<synthesis_abstract> hardware_raspi::update() {
	uint16_t knob = knob_->update();
	uint16_t select = knob & 0x100;
	uint8_t num = knob & 0xFF;
	if (select == 0x100) {
		// clang-format off
		switch (num) {
		case 0: return std::make_unique<synthesis_sin>();
		case 1: return std::make_unique<synthesis_saw>();
		case 2: return std::make_unique<synthesis_square>();
		case 3: return std::make_unique<synthesis_unison>();
		case 4: return std::make_unique<synthesis_hammond>();
		default: break;
		}
		// clang-format on
	}

	return nullptr;
}