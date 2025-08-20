#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include <spdlog/spdlog.h>
#include <CLI/CLI.hpp>

class command_abstract {
protected: /* file */
	int8_t read_binary(const std::string &path, std::vector<uint8_t> &binary);
	int8_t write_binary(const std::string &path, std::vector<uint8_t> &binary);
	int8_t read_text(const std::string &path, std::string &text);
	int8_t write_text(const std::string &path, std::string &text);

public: /* abstract */
	~command_abstract() {}

public: /* abstract */
	virtual void setup(CLI::App *parent) = 0;
	virtual void run() = 0;
};

class command_aes256_cbc : public command_abstract {
protected: /* parameter */
	std::string key;
	std::string iv;
	std::string in;
	std::string out;

public: /* abstract */
	void setup(CLI::App *parent) override final;
	void run() override final;
};