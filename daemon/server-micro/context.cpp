#include "daemon/server-micro/context.h"
#include "core/property.h"
#include "core/predefined.h"

void context_singleton::set_state(const machine::state &state) {
	state_ = state;
	return;
}

void context_singleton::set_presets(const nlohmann::json &presets) {
	presets_ = presets;
	return;
}

machine::state context_singleton::get_state() {
	return state_;
}

nlohmann::json context_singleton::get_presets() {
	return presets_;
}

void context_singleton::set_preset(uint8_t id) {
	for (const auto &object : presets_) {
		if (object.value("id", -1) == id) {
			preset_ = object.value("data", nlohmann::json());
			return;
		}
	}

	return;
}

nlohmann::json context_singleton::get_preset() {
	return std::move(preset_);
}

context_singleton &context_singleton::instance() {
	static context_singleton instance_;
	return instance_;
}

context_singleton::context_singleton() {
	std::string path = std::string(std::getenv("HOME")) + PATH_PRESET;
	std::ifstream ifs(path);
	if (ifs.is_open() == true) {
		ifs >> presets_;
		ifs.close();
	}
}