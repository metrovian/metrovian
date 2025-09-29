#pragma once
#include "command/abstract.h"
#include "command/optimization/user.h"

namespace sinusoidal {
enum class function : uint8_t;
};

class command_sinusoidal : public command_user {
protected: /* parameter */
	std::unordered_map<std::string, sinusoidal::function> map_;

public: /* abstract */
	virtual void setup(CLI::App *parent) override final;
	virtual void run() override final;
};