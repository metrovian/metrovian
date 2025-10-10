#pragma once
#include "abstract.h"

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

class command_aes256_gcm : public command_aes {
public: /* abstract */
	virtual void setup(CLI::App *parent) override final;
	virtual void run() override final;
};