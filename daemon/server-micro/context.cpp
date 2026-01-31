#include "daemon/server-micro/context.h"
#include "core/property.h"
#include "core/predefined.h"

machine::state context_singleton::apiget_state() {
	return state_;
}

nlohmann::ordered_json context_singleton::apiget_presets() {
	return presets_;
}

uint64_t context_singleton::apiget_progress() {
	return progress_;
}

uint64_t context_singleton::apiget_notes() {
	return CONFIG_UINT64("synthesis", "note-max") - CONFIG_UINT64("synthesis", "note-min");
}

void context_singleton::apiset_preset(uint8_t id) {
	for (const auto &object : presets_) {
		if (object.value("id", -1) == id) {
			preset_ = object.value("data", nlohmann::ordered_json());
			return;
		}
	}

	return;
}

nlohmann::ordered_json context_singleton::get_preset() {
	return std::move(preset_);
}

void context_singleton::set_state(const machine::state &state) {
	state_ = state;
	return;
}

void context_singleton::set_presets(const nlohmann::ordered_json &presets) {
	presets_ = presets;
	std::string path = std::string(std::getenv("HOME")) + PATH_PRESET;
	std::ofstream ofs(path);
	if (ofs.is_open() == true) {
		ofs << presets_.dump(8);
		ofs.close();
	}

	return;
}

void context_singleton::set_progress(uint64_t progress) {
	progress_ = progress;
	return;
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