#pragma once
#include "command/abstract.h"
#include "command/optimization/user.h"

namespace transcendental {
enum class function : uint8_t;
};

class command_transcendental : public command_user {
protected: /* parameter */
	std::unordered_map<std::string, transcendental::function> map_;

public: /* abstract */
	virtual void setup(CLI::App *parent) override final;
	virtual void run() override final;
};