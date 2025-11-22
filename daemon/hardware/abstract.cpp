#include "daemon/hardware/abstract.h"
#include "core/predefined.h"

std::unique_ptr<synthesis_abstract> hardware_abstract::read() {
	return nullptr;
}

void hardware_abstract::enter(machine::state state) {
	return;
}

void hardware_abstract::exit(machine::state state) {
	return;
}