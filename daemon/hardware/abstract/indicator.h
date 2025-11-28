#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <atomic>
#include <thread>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <gpiod.h>

namespace indicator {
enum class state : uint8_t {
	off = 0,
	on = 1,
	blink_slow = 2,
	blink_fast = 3,
};
}; // namespace indicator

class indicator_abstract {
public: /* destructor */
	virtual ~indicator_abstract() {}

public: /* abstract */
	virtual void set(indicator::state state) = 0;
};