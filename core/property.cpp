#include "core/property.h"
#include "core/predefined.h"

property_singleton &property_singleton::instance() {
	static property_singleton instance_;
	return instance_;
}

std::string property_singleton::parse(std::vector<std::string> property) {
	nlohmann::ordered_json json = parser_;
	for (const auto &key : property) {
		if (!json.contains(key)) {
			return std::string();
		}

		json = json[key];
	}

	if (json.is_number_unsigned()) {
		return std::to_string(json.get<uint64_t>());
	} else if (json.is_number_integer()) {
		return std::to_string(json.get<int64_t>());
	} else if (json.is_number_float()) {
		return std::to_string(json.get<double>());
	} else if (json.is_boolean()) {
		return json.get<bool>() ? std::string("true") : std::string("false");
	} else if (json.is_string()) {
		return json.get<std::string>();
	}

	return json.dump();
}

property_singleton::property_singleton() {
	std::string dir = std::string(std::getenv("CONFIGURATION_DIRECTORY"));
	if (access(dir.c_str(), F_OK) != 0) {
		if (mkdir(dir.c_str(), 0755) != 0) {
			LOG_CONDITION(mkdir != 0);
			return;
		}
	}

	load_default();
	std::string path = std::string(std::getenv("CONFIGURATION_DIRECTORY")) + CONFIG_PROPERTY;
	std::ifstream ifs(path);
	if (ifs.is_open() == true) {
		ifs >> parser_;
		ifs.close();
	} else {
		parser_ = default_;
	}

	merge_default(parser_, default_);
	std::ofstream ofs(path);
	if (ofs.is_open() == true) {
		ofs << parser_.dump(8);
		ofs.close();
	}
}

void property_singleton::load_default() {
	/* hardware */
	default_["alsa"]["player"]["name"] = "default";
	default_["alsa"]["mixer"]["name"] = "default";
	default_["alsa"]["sequencer"]["name"] = "default";
	default_["alsa"]["sequencer"]["mux-length"] = 32;
	/* core */
	default_["optimization"]["thread-max"] = 8;
	default_["optimization"]["derivative-step"] = 1.000E-5;
	default_["optimization"]["damp-step"] = 1.000E-3;
	default_["optimization"]["increase-step"] = 1.000E+1;
	default_["optimization"]["decrease-step"] = 1.000E+1;
	default_["synthesis"]["channel"] = 1;
	default_["synthesis"]["sample-rate"] = 44100;
	default_["synthesis"]["note-min"] = 21;
	default_["synthesis"]["note-max"] = 109;
	default_["synthesis"]["tune"] = 440;
	default_["synthesis"]["period"] = 10;
}

void property_singleton::merge_default(nlohmann::ordered_json &target, const nlohmann::ordered_json &source) {
	for (auto &[key, value] : source.items()) {
		if (target.contains(key) == false) {
			target[key] = value;
		} else if (target[key].is_object() == true) {
			if (value.is_object() == true) {
				merge_default(target[key], value);
			}
		}
	}
}