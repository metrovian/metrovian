#include "daemon/main.h"
#include "core/predefined.h"

machine_singleton &machine_singleton::instance() {
	static machine_singleton instance_;
	return instance_;
}

void machine_singleton::transition(machine::state next) {
	LOG_ENTER();
	hw_->exit(state_.load());
	hw_->enter(next);
	state_.store(next);
	map_[state_.load()]->enter();
	LOG_EXIT();
	return;
}

void machine_singleton::loop() {
	LOG_ENTER();
	transition(machine::state::setup);
	while (state_.load() != machine::state::shutdown) {
		map_[state_.load()]->update();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	state_.store(machine::state::startup);
	LOG_EXIT();
	return;
}

void machine_singleton::shutdown() {
	LOG_ENTER();
	state_.store(machine::state::shutdown);
	while (state_.load() != machine::state::startup) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		continue;
	}

	core_.reset();
	hw_.reset();
	LOG_EXIT();
	return;
}

void machine_singleton::load_hardware() {
	hw_ = std::make_unique<hardware_raspi>();
	hw_->create();
	return;
}

void machine_singleton::load_map() {
	map_.insert(std::make_pair(machine::state::startup, std::make_unique<state_dummy>()));
	map_.insert(std::make_pair(machine::state::shutdown, std::make_unique<state_dummy>()));
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
	LOG_ENTER();
	load_hardware();
	load_map();
	load_stdout();
	load_stderr();
	LOG_EXIT();
}

static void handle_terminate(int) {
	machine_singleton::instance().shutdown();
	return;
}

int main(int, char **) {
	std::signal(SIGTERM, handle_terminate);
	std::signal(SIGINT, handle_terminate);
	machine_singleton::instance().loop();
	return 0;
}