#include "daemon/state/setup.h"
#include "daemon/main.h"
#include "core/synthesis/hammond.h"

void state_setup::enter() {
}

void state_setup::update() {
	// TODO: select by hardware configuration
	machine_singleton::instance().setup(std::make_unique<synthesis_hammond>());
	machine_singleton::instance().transition(machine::state::synthesis);
	return;
}