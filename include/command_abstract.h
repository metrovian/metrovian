#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <CLI/CLI.hpp>

#include "decryption/aes.h"
#include "decryption/rsa.h"
#include "decompression/avcodec.h"
#include "optimization/user.h"

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
	virtual void setup(CLI::App *parent) override final;
	virtual void run() override final;
};

class command_aes256_ctr : public command_aes {
public: /* abstract */
	virtual void setup(CLI::App *parent) override final;
	virtual void run() override final;
};

class command_rsa : public command_abstract {
protected: /* parameter */
	std::string private_pem_;
	std::string in_;
	std::string out_;

public: /* abstract */
	virtual void setup(CLI::App *parent) override;
	virtual void run() override;
};

class command_rsa_attack : public command_rsa {
protected: /* parameter */
	std::string public_pem_;
	std::string method_;
	std::unordered_map<std::string, rsa::attack> map_;

public: /* abstract */
	virtual void setup(CLI::App *parent) override final;
	virtual void run() override final;
};

class command_avcodec : public command_abstract {
protected: /* parameter */
	std::string in_;

public: /* abstract */
	virtual void setup(CLI::App *parent) override final;
	virtual void run() override final;
};

class command_user : public command_abstract {
protected: /* parameter */
	std::string function_;
	std::string in_;
	size_t iter_;
	double eps_;

public: /* abstract */
	virtual void setup(CLI::App *parent) override final;
	virtual void run() override final;
};