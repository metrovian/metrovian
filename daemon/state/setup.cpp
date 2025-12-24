#include "daemon/state/setup.h"
#include "daemon/server-micro/context.h"
#include "daemon/main.h"
#include "core/synthesis/abstract.h"

void state_setup::enter() {
	machine_singleton::instance().setup();
	return;
}

void state_setup::update() {
	nlohmann::json preset = context_singleton::instance().get_preset();
	if (preset.empty() == false) {
		machine_singleton::instance().setup(preset);
		machine_singleton::instance().transition(machine::state::synthesis);
	}

	return;
}