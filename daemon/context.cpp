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

nlohmann::ordered_json context_api::read_presets() {
	return context_singleton::instance().presets_;
}

nlohmann::ordered_json context_api::read_mids() {
	nlohmann::ordered_json mids;
	std::error_code code;
	std::filesystem::path dir(std::getenv("STATE_DIRECTORY"));
	for (const auto &entry : std::filesystem::directory_iterator(dir, code)) {
		if (code.value() == 0) {
			if (entry.is_regular_file() == false) {
				continue;
			}

			auto path = entry.path();
			auto ext = path.extension().string();
			if (ext == ".mid") {
				nlohmann::ordered_json metadata;
				metadata["filename"] = path.filename().string();
				metadata["title"] = path.stem().string();
				metadata["composer"] = nullptr;
				mids.push_back(metadata);
			}
		}
	}

	return mids;
}

std::string context_api::read_mid() {
	return context_singleton::instance().mid_;
}

uint8_t context_api::read_state() {
	uint8_t state = static_cast<uint8_t>(context_singleton::instance().state_.load());
	return state;
}

uint8_t context_api::read_id() {
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

machine::state context_machine::read_state() {
	return context_singleton::instance().state_.load();
}

nlohmann::ordered_json context_machine::read_preset() {
	return std::move(context_singleton::instance().preset_);
}

void context_machine::write_mid(const std::string mid) {
	context_singleton::instance().mid_ = mid;
	return;
}

void context_machine::write_state(const machine::state state) {
	context_singleton::instance().state_.store(state);
	return;
}

void context_machine::write_progress(const uint64_t progress) {
	context_singleton::instance().progress_.store(progress);
	return;
}