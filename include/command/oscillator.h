#pragma once
#include "command/abstract.h"
#include "command/user.h"

namespace oscillator {
enum class function : uint8_t;
};

class command_oscillator : public command_user {
protected: /* parameter */
	std::unordered_map<std::string, oscillator::function> map_;

public: /* abstract */
	virtual void setup(CLI::App *parent) override final;
	virtual void run() override final;
};