#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cmath>
#include <csignal>
#include <thread>
#include <chrono>
#include <functional>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <nlohmann/json.hpp>

namespace machine {
enum class state : uint8_t {
	none = 0,
	setup = 1,
	synthesis = 2,
	performance = 3,
	shutdown = 4,
};
}; // namespace machine

class state_abstract {
public: /* abstract */
	virtual ~state_abstract() {}

public: /* abstract */
	virtual void enter() = 0;
	virtual void update() = 0;
};