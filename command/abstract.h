#pragma once
#include <CLI/CLI.hpp>
#include <Eigen/Dense>

class command_abstract {
public:
	virtual ~command_abstract() = default;
	virtual void setup(CLI::App *parent) = 0;
	virtual void run() = 0;

protected:
	static void handle_setup(const std::function<void(void)> handler);
	static void handle_terminate(int);
	static int read_binary(const std::string &path, std::vector<uint8_t> &binary);
	static int write_binary(const std::string &path, std::vector<uint8_t> &binary);
	static int read_text(const std::string &path, std::string &text);
	static int write_text(const std::string &path, std::string &text);
	static int read_vector(const std::string &path, Eigen::VectorXd &range);
	static int write_vector(const std::string &path, Eigen::VectorXd &range);
	static int read_vector(const std::string &path, Eigen::VectorXd &domain, Eigen::VectorXd &range, char delimiter);
	static int write_vector(const std::string &path, Eigen::VectorXd &domain, Eigen::VectorXd &range, char delimiter);

private:
	int setup_subcommand(std::unique_ptr<command_abstract> command);
	int select_subcommand();

private:
	static inline std::function<void(void)> handler_ = nullptr;

private:
	CLI::App *command_parser_;
	std::vector<std::unique_ptr<command_abstract>> commands_;
};