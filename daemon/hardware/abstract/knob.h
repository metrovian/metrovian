#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <atomic>
#include <termios.h>
#include <unistd.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

class knob_abstract {
public: /* abstract */
	virtual void start() = 0;
	virtual void stop() = 0;

public: /* abstract */
	virtual uint16_t update() = 0;
};