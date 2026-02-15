#include "daemon/machine/state/abstract.h"
#include "daemon/main.h"

void state_synthesis::enter() {
	complete_.store(0);
	std::thread([&]() {
		machine_singleton::instance().synthesize();
		complete_.store(1);
	}).detach();

	return;
}

void state_synthesis::update() {
	if (complete_.load() != 0) {
		machine_singleton::instance().transition(machine::state::performance);
	}

	return;
}
