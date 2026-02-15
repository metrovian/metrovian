#include "daemon/state/abstract.h"
#include "daemon/context.h"
#include "daemon/main.h"

void state_setup::enter() {
	machine_singleton::instance().setup();
	return;
}

void state_setup::update() {
	nlohmann::ordered_json preset = context_main::read_preset();
	if (preset.empty() == false) {
		machine_singleton::instance().setup(preset);
		machine_singleton::instance().transition(machine::state::synthesis);
	}

	return;
}