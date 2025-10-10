#pragma once
#include "abstract.h"
#include "optimization/user.h"

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