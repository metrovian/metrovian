#include "daemon/main.h"
#include "core/property.h"
#include "core/predefined.h"

void machine_singleton::setup(const nlohmann::ordered_json &preset) {
	LOG_ENTER();
	std::string method = preset.value("method", "");
	if (method == std::string("add")) {
		core_ = std::make_unique<synthesis_add>(preset);
	}

	LOG_EXIT();
	return;
}

void machine_singleton::setup() {
	core_.reset();
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
	context_singleton::instance().set_state(next);
	state_.store(next);
	smap_[state_.load()]->enter();
	LOG_EXIT();
	return;
}

void machine_singleton::loop() {
	LOG_ENTER();
	transition(machine::state::setup);
	while (state_.load() != machine::state::shutdown) {
		smap_[state_.load()]->update();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	LOG_EXIT();
	return;
}

void machine_singleton::shutdown() {
	state_.store(machine::state::shutdown);
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

void machine_singleton::load_smap() {
	smap_.insert(std::make_pair(machine::state::none, std::make_unique<state_none>()));
	smap_.insert(std::make_pair(machine::state::shutdown, std::make_unique<state_none>()));
	smap_.insert(std::make_pair(machine::state::setup, std::make_unique<state_setup>()));
	smap_.insert(std::make_pair(machine::state::synthesis, std::make_unique<state_synthesis>()));
	smap_.insert(std::make_pair(machine::state::performance, std::make_unique<state_performance>()));
	return;
}

machine_singleton::machine_singleton() {
	load_stdout();
	load_stderr();
	load_smap();
	handle_setup([&]() { shutdown(); });
	property_singleton::instance();
	context_singleton::instance();
	api_singleton::instance();
}

int main(int, char **) {
	std::jthread([&]() { machine_singleton::instance().loop(); });
	return 0;
}