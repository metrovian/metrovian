#include "daemon/state/setup.h"
#include "daemon/main.h"
#include "core/synthesis/abstract.h"

void state_setup::enter() {
	machine_singleton::instance().core_.reset();
	return;
}

void state_setup::update() {
	machine_singleton::instance().core_ = machine_singleton::instance().hw_->update();
	if (machine_singleton::instance().core_ != nullptr) {
		machine_singleton::instance().transition(machine::state::synthesis);
	}

	return;
}