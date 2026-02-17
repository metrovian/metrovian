#pragma once
#include "command/abstract.h"

class command_audio final : public command_abstract {
public:
	void setup(CLI::App *parent) override;
	void run() override;

private:
	std::string in_;
	std::string out_;
};