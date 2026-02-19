#pragma once
#include "command/abstract.h"

class command_correlation final : public command_abstract {
public:
	void setup(CLI::App *parent) override;
	void run() override;

private:
	std::vector<std::string> in_;
	std::string out_;
};

class command_fourier final : public command_abstract {
public:
	void setup(CLI::App *parent) override;
	void run() override;

private:
	std::string in_;
	std::string out_;
};