#include "core/predefined.h"

extern std::string CONFIG_PATH() {
	const char *env = std::getenv("CONFIGURATION_DIRECTORY");
	if (env == nullptr) {
		return std::string("/etc/metrovian");
	}

	return std::string(env);
}

extern std::string CONFIG_PATH(std::string name) {
	return CONFIG_PATH() + name;
}