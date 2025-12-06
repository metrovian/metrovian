#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cmath>
#include <csignal>
#include <thread>
#include <chrono>
#include <functional>
#include <sys/utsname.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <CLI/CLI.hpp>
#include <Eigen/Dense>

namespace machine {
enum class state : uint8_t {
	none = 0,
	setup = 1,
	synthesis = 2,
	performance = 3,
	shutdown = 4,
};

enum class waveform : uint8_t {
	none = 0,
	sin = 1,
	saw = 2,
	square = 3,
	unison = 4,
	hammond = 5,
};
}; // namespace machine

class state_abstract {
public: /* abstract */
	virtual ~state_abstract() {}

public: /* abstract */
	virtual void enter() = 0;
	virtual void update() = 0;
};