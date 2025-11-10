#pragma once
#include "command/abstract.h"
#include "command/optimization/user.h"

class command_rational : public command_user {
protected: /* parameter */
	std::vector<uint8_t> model_;

public: /* abstract */
	virtual void setup(CLI::App *parent) override final;
	virtual void run() override final;
};