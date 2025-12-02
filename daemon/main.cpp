#include "daemon/main.h"
#include "core/predefined.h"

void machine_singleton::handle_setup(const std::function<void(void)> handler) {
	handler_ = handler;
	if (handler_ != nullptr) {
		std::signal(SIGTERM, machine_singleton::handle_terminate);
		std::signal(SIGINT, machine_singleton::handle_terminate);
	}

	return;
}

void machine_singleton::handle_terminate(int) {
	if (handler_ != nullptr) {
		handler_();
	}

	return;
}

machine_singleton &machine_singleton::instance() {
	static machine_singleton instance_;
	return instance_;
}

void machine_singleton::transition(machine::state next) {
	LOG_ENTER();
	context_singleton::instance().transition(next);
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

	core_.reset();
	hw_.reset();
	LOG_EXIT();
	return;
}

void machine_singleton::shutdown() {
	state_.store(machine::state::shutdown);
	return;
}

void machine_singleton::load_hardware() {
	struct utsname result;
	if (uname(&result) != 0) {
		return;
	}

	std::string platform = result.machine;
	if (platform == "aarch64" ||
	    platform == "armv7l" ||
	    platform == "armv6l") {
		hw_ = std::make_unique<hardware_raspi>();
	} else {
		hw_ = std::make_unique<hardware_development>();
	}

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
	load_hardware();
	load_map();
	load_stdout();
	load_stderr();
	handle_setup([&]() { shutdown(); });
	context_singleton::instance();
	api_singleton::instance();
}

int main(int, char **) {
	std::jthread([&]() { machine_singleton::instance().loop(); });
	return 0;
}