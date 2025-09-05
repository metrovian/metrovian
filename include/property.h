#pragma once
#include <nlohmann/json.hpp>

class property_singleton {
protected: /* json parser */
	nlohmann::json parser_;

public: /* json parser */
	std::string parse(std::vector<std::string> property);

public: /* instance */
	static property_singleton &instance();

private: /* constraint */
	property_singleton();
	property_singleton(const property_singleton &) = default;
	property_singleton &operator=(const property_singleton &) = default;
};