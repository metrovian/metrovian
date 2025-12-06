#include "main.h"
#include "core/predefined.h"

void machine_singleton::transition(machine::state next) {
	LOG_ENTER();
	context_singleton::instance().transition(next);
	state_.store(next);
	smap_[state_.load()]->enter();
	LOG_EXIT();
	return;
}

void machine_singleton::transition(machine::waveform next) {
	LOG_ENTER();
	context_singleton::instance().transition(next);
	waveform_.store(next);
	LOG_EXIT();
	return;
}

void machine_singleton::synthesize() {
	wmap_[waveform_.load()]->synthesize();
	return;
}

void machine_singleton::perform() {
	wmap_[waveform_.load()]->perform();
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
	smap_.insert(std::make_pair(machine::state::none, std::make_unique<state_dummy>()));
	smap_.insert(std::make_pair(machine::state::shutdown, std::make_unique<state_dummy>()));
	smap_.insert(std::make_pair(machine::state::setup, std::make_unique<state_setup>()));
	smap_.insert(std::make_pair(machine::state::synthesis, std::make_unique<state_synthesis>()));
	smap_.insert(std::make_pair(machine::state::performance, std::make_unique<state_performance>()));
	return;
}

void machine_singleton::load_wmap() {
	wmap_.insert(std::make_pair(machine::waveform::none, nullptr));
	wmap_.insert(std::make_pair(machine::waveform::sin, std::make_unique<synthesis_sin>()));
	wmap_.insert(std::make_pair(machine::waveform::saw, std::make_unique<synthesis_saw>()));
	wmap_.insert(std::make_pair(machine::waveform::square, std::make_unique<synthesis_square>()));
	wmap_.insert(std::make_pair(machine::waveform::unison, std::make_unique<synthesis_unison>()));
	wmap_.insert(std::make_pair(machine::waveform::hammond, std::make_unique<synthesis_hammond>()));
	return;
}

machine_singleton::machine_singleton() {
	load_stdout();
	load_stderr();
	load_smap();
	load_wmap();
	handle_setup([&]() { shutdown(); });
	context_singleton::instance();
	api_singleton::instance();
}

int main(int, char **) {
	std::jthread([&]() { machine_singleton::instance().loop(); });
	return 0;
}