#pragma once
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

class property_singleton {
public:
	static property_singleton &instance();

public:
	property_singleton(const property_singleton &) = delete;
	property_singleton(property_singleton &&) = delete;
	property_singleton &operator=(const property_singleton &) = delete;
	property_singleton &operator=(property_singleton &&) = delete;
	std::string parse(std::vector<std::string> property);

private:
	~property_singleton() = default;
	property_singleton();
	void load_default();
	void merge_default(nlohmann::ordered_json &target, const nlohmann::ordered_json &source);

private:
	nlohmann::ordered_json parser_;
	nlohmann::ordered_json default_;
};