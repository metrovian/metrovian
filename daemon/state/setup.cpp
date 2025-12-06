#include "daemon/state/setup.h"
#include "daemon/server-micro/context.h"
#include "daemon/main.h"
#include "core/synthesis/abstract.h"

void state_setup::enter() {
	machine_singleton::instance().transition(machine::waveform::none);
	return;
}

void state_setup::update() {
	machine::waveform waveform = context_singleton::instance().update_waveform();
	if (waveform != machine::waveform::none) {
		machine_singleton::instance().transition(waveform);
		machine_singleton::instance().transition(machine::state::synthesis);
	}

	return;
}