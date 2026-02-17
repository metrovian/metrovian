#pragma once
#include "command/abstract.h"

class command_user : public command_abstract {
public:
	void setup(CLI::App *parent) override;
	void run() override;

protected:
	std::string func_;
	std::string in_;
	size_t iter_;
	double eps_;
	std::vector<double> params_;
};

class command_polynomial final : public command_user {
public:
	void setup(CLI::App *parent) override;
	void run() override;

private:
	uint8_t model_;
};

class command_rational final : public command_user {
public:
	void setup(CLI::App *parent) override;
	void run() override;

private:
	std::vector<uint8_t> model_;
};

class command_sinusoidal final : public command_user {
public:
	void setup(CLI::App *parent) override;
	void run() override;
};

class command_transcendental final : public command_user {
public:
	void setup(CLI::App *parent) override;
	void run() override;
};