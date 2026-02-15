#include "daemon/main.h"
#include "daemon/api.h"
#include "daemon/automata.h"
#include "daemon/context.h"
#include "core/property.h"
#include "core/predefined.h"

machine_singleton &machine_singleton::instance() {
	static machine_singleton instance_;
	return instance_;
}

void machine_singleton::transition(machine::state next) {
	context_main::write_state(next);
	smap_[next]->enter();
	return;
}

void machine_singleton::loop() {
	LOG_ENTER();
	transition(machine::state::setup);
	while (context_main::read_state() != machine::state::shutdown) {
		smap_[context_main::read_state()]->update();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	LOG_EXIT();
	return;
}

void machine_singleton::shutdown() {
	context_main::write_state(machine::state::shutdown);
	return;
}

void machine_singleton::panic() {
	core_->panic();
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

machine_singleton::machine_singleton() {
	std::cout << std::scientific << std::showpos << std::setprecision(6);
	spdlog::set_default_logger(spdlog::stderr_color_mt("stderr"));
	spdlog::set_level(spdlog::level::info);
	smap_.insert(std::make_pair(machine::state::none, std::make_unique<state_none>()));
	smap_.insert(std::make_pair(machine::state::shutdown, std::make_unique<state_none>()));
	smap_.insert(std::make_pair(machine::state::setup, std::make_unique<state_setup>()));
	smap_.insert(std::make_pair(machine::state::synthesis, std::make_unique<state_synthesis>()));
	smap_.insert(std::make_pair(machine::state::performance, std::make_unique<state_performance>()));
	property_singleton::instance();
	context_singleton::instance();
	automata_singleton::instance();
	api_singleton::instance();
	handle_setup([&]() { shutdown(); });
}

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
	core_->callback_synthesis([&](uint64_t value) {
		context_main::write_progress(value);
	});

	core_->synthesize();
	return;
}

void machine_singleton::perform() {
	core_->perform();
	return;
}

int main(int, char **) {
	std::jthread([&]() { machine_singleton::instance().loop(); });
	return 0;
}