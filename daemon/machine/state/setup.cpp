#include "daemon/machine/state/abstract.h"
#include "daemon/context.h"
#include "daemon/main.h"

#include <nlohmann/json.hpp>

void state_setup::enter() {
	machine_singleton::instance().setup();
	return;
}

void state_setup::update() {
	nlohmann::ordered_json preset = context_machine::read_preset();
	if (preset.empty() == false) {
		machine_singleton::instance().setup(preset);
		machine_singleton::instance().transition(machine::state::synthesis);
	}

	return;
}