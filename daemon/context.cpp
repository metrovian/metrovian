#include "daemon/context.h"
#include "core/property.h"
#include "core/predefined.h"

context_singleton &context_singleton::instance() {
	static context_singleton instance_;
	return instance_;
}

context_singleton::context_singleton() {
	std::string path = std::string(std::getenv("CONFIGURATION_DIRECTORY")) + CONFIG_PRESET;
	std::ifstream ifs(path);
	if (ifs.is_open() == true) {
		ifs >> presets_;
		ifs.close();
	}
}

machine::state context_api::read_state() {
	return context_singleton::instance().state_.load();
}

nlohmann::ordered_json context_api::read_presets() {
	return context_singleton::instance().presets_;
}

uint8_t context_api::read_preset() {
	return context_singleton::instance().id_.load();
}

uint64_t context_api::read_progress() {
	return context_singleton::instance().progress_.load();
}

uint64_t context_api::read_notes() {
	return CONFIG_UINT64("synthesis", "note-max") - CONFIG_UINT64("synthesis", "note-min");
}

void context_api::write_presets(const nlohmann::ordered_json &presets) {
	context_singleton::instance().presets_ = presets;
	std::string path = std::string(std::getenv("CONFIGURATION_DIRECTORY")) + CONFIG_PRESET;
	std::ofstream ofs(path);
	if (ofs.is_open() == true) {
		ofs << presets.dump(8);
		ofs.close();
	}

	return;
}

void context_api::write_preset(const uint8_t id) {
	context_singleton *instance = &context_singleton::instance();
	for (const auto &object : instance->presets_) {
		if (object.value("id", -1) == id) {
			instance->id_.store(id);
			instance->preset_ = object.value("data", nlohmann::ordered_json());
			return;
		}
	}

	return;
}

machine::state context_main::read_state() {
	return context_singleton::instance().state_.load();
}

nlohmann::ordered_json context_main::read_preset() {
	return std::move(context_singleton::instance().preset_);
}

void context_main::write_state(const machine::state state) {
	context_singleton::instance().state_.store(state);
	return;
}

void context_main::write_progress(const uint64_t progress) {
	context_singleton::instance().progress_.store(progress);
	return;
}