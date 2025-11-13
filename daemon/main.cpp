#include "daemon/main.h"

machine_singleton &machine_singleton::instance() {
	static machine_singleton instance_;
	return instance_;
}

machine_singleton::machine_singleton() {
	map_.insert(std::make_pair(machine::state::wait, std::make_unique<state_wait>()));
	map_.insert(std::make_pair(machine::state::wait, std::make_unique<state_synthesis>()));
	map_.insert(std::make_pair(machine::state::wait, std::make_unique<state_performance>()));
	map_[state_.load()]->enter();
}

int32_t main(int argc, char **argv) {
	machine_singleton::instance();
	return 0;
}