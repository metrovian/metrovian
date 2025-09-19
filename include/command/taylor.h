#pragma once
#include "command/abstract.h"
#include "command/user.h"

class command_taylor : public command_user {
protected: /* parameter */
	uint8_t model_;

public: /* abstract */
	virtual void setup(CLI::App *parent) override final;
	virtual void run() override final;
};