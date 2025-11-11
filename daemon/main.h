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

class machine_singleton {
public: /* instance */
	static machine_singleton &instance();

private: /* constraint */
	machine_singleton() {}
	machine_singleton(const machine_singleton &) = default;
	machine_singleton &operator=(const machine_singleton &) = default;
};