#include "property.h"
#include "predefined.h"

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

property_singleton &property_singleton::instance() {
	static property_singleton instance_;
	return instance_;
}

void property_singleton::load_default() {
	/* hardware */
	default_["alsa"]["player"]["name"] = "default";
	default_["alsa"]["sequencer"]["name"] = "default";
	default_["alsa"]["sequencer"]["client"] = 20;
	default_["alsa"]["sequencer"]["port"] = 0;
	default_["alsa"]["sequencer"]["mux-length"] = 32;
	/* core */
	default_["decryption"]["rsa"]["trial-iteration"] = 1000000;
	default_["decryption"]["rsa"]["fermat-iteration"] = 1000000;
	default_["decryption"]["rsa"]["pollards-rho-iteration"] = 1000000;
	default_["decryption"]["rsa"]["pollards-p1-iteration"] = 100000;
	default_["decryption"]["rsa"]["williams-p1-iteration"] = 100000;
	default_["decryption"]["ecdsa"]["trial-iteration"] = 1000000;
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

property_singleton::property_singleton() {
	load_default();
	std::string path = std::string(std::getenv("HOME")) + PATH_PROPERTY;
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