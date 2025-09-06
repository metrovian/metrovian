#include "property.h"
#include "predefined.h"

std::string property_singleton::parse(std::vector<std::string> property) {
	nlohmann::json json = parser_;
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

property_singleton::property_singleton() {
	std::string path = std::string(std::getenv("HOME")) + PATH_PROPERTY;
	std::ifstream ifs(path);
	if (ifs.is_open() == true) {
		ifs >> parser_;
		ifs.close();
	} else {
		std::ofstream ofs(path);
		if (ofs.is_open() == true) {
			parser_["decryption"]["rsa"]["fermat-iteration"] = 1000000;
			parser_["decryption"]["rsa"]["pollards-rho-iteration"] = 100000000;
			parser_["decryption"]["rsa"]["pollards-p1-iteration"] = 100000;
			parser_["decryption"]["rsa"]["williams-p1-iteration"] = 100000;
			ofs << parser_.dump(8);
			ofs.close();
		}
	}
}