#include "daemon/state/abstract.h"
#include "daemon/server-micro/context.h"
#include "daemon/main.h"

void state_setup::enter() {
	machine_singleton::instance().setup();
	return;
}

void state_setup::update() {
	nlohmann::ordered_json preset = context_singleton::instance().get_preset();
	if (preset.empty() == false) {
		machine_singleton::instance().setup(preset);
		machine_singleton::instance().transition(machine::state::synthesis);
	}

	return;
}