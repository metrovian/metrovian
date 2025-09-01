#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <CLI/CLI.hpp>
#include <Eigen/Dense>

class command_abstract {
protected: /* command parser */
	CLI::App *command_parser_;
	std::vector<std::unique_ptr<command_abstract>> commands_;

protected: /* file */
	int8_t read_binary(const std::string &path, std::vector<uint8_t> &binary);
	int8_t write_binary(const std::string &path, std::vector<uint8_t> &binary);
	int8_t read_text(const std::string &path, std::string &text);
	int8_t write_text(const std::string &path, std::string &text);
	int8_t read_vector(const std::string &path, Eigen::VectorXd &domain, Eigen::VectorXd &range, char delimiter);
	int8_t read_write(const std::string &path, Eigen::VectorXd &domain, Eigen::VectorXd &range, char delimiter);

protected: /* subcommand */
	int8_t setup_subcommand(std::unique_ptr<command_abstract> command);
	int8_t select_subcommand();

public: /* abstract */
	~command_abstract() {}

public: /* abstract */
	virtual void setup(CLI::App *parent) = 0;
	virtual void run() = 0;
};