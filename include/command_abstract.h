#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include <spdlog/spdlog.h>
#include <Eigen/Dense>
#include <CLI/CLI.hpp>

class command_abstract {
protected: /* file */
	int8_t read_binary(const std::string &path, std::vector<uint8_t> &binary);
	int8_t write_binary(const std::string &path, std::vector<uint8_t> &binary);
	int8_t read_text(const std::string &path, std::string &text);
	int8_t write_text(const std::string &path, std::string &text);
	int8_t read_vector(const std::string &path, Eigen::VectorXd &domain, Eigen::VectorXd &range, char delimiter);
	int8_t read_write(const std::string &path, Eigen::VectorXd &domain, Eigen::VectorXd &range, char delimiter);

public: /* abstract */
	~command_abstract() {}

public: /* abstract */
	virtual void setup(CLI::App *parent) = 0;
	virtual void run() = 0;
};

class command_aes : public command_abstract {
protected: /* parameter */
	std::string key_;
	std::string iv_;
	std::string in_;
	std::string out_;
};

class command_aes256_cbc : public command_aes {
public: /* abstract */
	void setup(CLI::App *parent) override final;
	void run() override final;
};

class command_aes256_ctr : public command_aes {
public: /* abstract */
	void setup(CLI::App *parent) override final;
	void run() override final;
};

class command_rsa : public command_abstract {
protected: /* parameter */
	std::string private_pem_;
	std::string in_;
	std::string out_;

public: /* abstract */
	void setup(CLI::App *parent) override final;
	void run() override final;
};

class command_avcodec : public command_abstract {
protected: /* parameter */
	std::string in_;

public: /* abstract */
	void setup(CLI::App *parent) override final;
	void run() override final;
};

class command_user : public command_abstract {
protected: /* parameter */
	std::string function_;
	std::string in_;
	size_t iter_;
	double eps_;

public: /* abstract */
	void setup(CLI::App *parent) override final;
	void run() override final;
};