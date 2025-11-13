#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cmath>
#include <csignal>
#include <functional>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <CLI/CLI.hpp>
#include <Eigen/Dense>

namespace machine {
enum class state : uint8_t {
	wait = 0,
	synthesis = 1,
	performance = 2,
};
}; // namespace machine

class state_abstract {
public: /* abstract */
	virtual ~state_abstract() {}

public: /* abstract */
	virtual void enter() = 0;
	virtual void exit() = 0;
	virtual void update() = 0;
};