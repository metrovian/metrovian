#include "daemon/main.h"

machine_singleton &machine_singleton::instance() {
	static machine_singleton instance_;
	return instance_;
}

void machine_singleton::transition(machine::state next) {
	hw_->exit(state_.load());
	hw_->enter(next);
	state_.store(next);
	map_[state_.load()]->enter();
	return;
}

void machine_singleton::loop() {
	transition(machine::state::setup);
	while (state_.load() != machine::state::startup) {
		map_[state_.load()]->update();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	return;
}

void machine_singleton::load_hardware() {
	hw_ = std::make_unique<hardware_development>();
	hw_->create();
	return;
}

void machine_singleton::load_map() {
	map_.insert(std::make_pair(machine::state::setup, std::make_unique<state_setup>()));
	map_.insert(std::make_pair(machine::state::synthesis, std::make_unique<state_synthesis>()));
	map_.insert(std::make_pair(machine::state::performance, std::make_unique<state_performance>()));
	return;
}

void machine_singleton::load_stdout() {
	std::cout << std::scientific << std::showpos << std::setprecision(6);
	return;
}

void machine_singleton::load_stderr() {
	spdlog::set_default_logger(spdlog::stderr_color_mt("stderr"));
	spdlog::set_level(spdlog::level::trace);
	return;
}

machine_singleton::machine_singleton() {
	load_hardware();
	load_map();
	load_stdout();
	load_stderr();
}

int32_t main() {
	machine_singleton::instance().loop();
	return 0;
}