#include "daemon/state/performance.h"
#include "daemon/main.h"
#include "core/predefined.h"

void state_performance::enter() {
	complete_.store(0);
	std::thread([&]() {
		machine_singleton::instance().core_->perform();
		complete_.store(1);
	}).detach();

	return;
}

void state_performance::update() {
	if (complete_.load() != 0) {
		machine_singleton::instance().transition(machine::state::setup);
	}

	return;
}
