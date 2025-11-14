#include "daemon/main.h"

void machine_singleton::setup(std::unique_ptr<synthesis_abstract> core) {
	core_ = std::move(core);
	return;
}

void machine_singleton::synthesize() {
	core_->synthesize();
	return;
}

void machine_singleton::perform() {
	core_->perform();
	return;
}

void machine_singleton::terminate() {
	core_->terminate();
	return;
}

void machine_singleton::run() {
	transition(machine::state::setup);
	while (state_.load() != machine::state::startup) {
		map_[state_.load()]->update();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	return;
}

void machine_singleton::transition(machine::state next) {
	state_.store(next);
	map_[state_.load()]->enter();
	return;
}

machine_singleton &machine_singleton::instance() {
	static machine_singleton instance_;
	return instance_;
}

machine_singleton::machine_singleton() {
	map_.insert(std::make_pair(machine::state::setup, std::make_unique<state_setup>()));
	map_.insert(std::make_pair(machine::state::synthesis, std::make_unique<state_synthesis>()));
	map_.insert(std::make_pair(machine::state::performance, std::make_unique<state_performance>()));
}

int32_t main(int argc, char **argv) {
	machine_singleton::instance().run();
	return 0;
}