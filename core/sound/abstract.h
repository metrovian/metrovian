#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <atomic>
#include <condition_variable>
#include <map>
#include <mutex>
#include <cmath>
#include <string>
#include <queue>
#include <vector>
#include <functional>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

class sound_abstract {
protected: /* parameter */
	uint16_t channel_ = 0;
	uint32_t sample_rate_ = 0;

public: /* constructor */
	sound_abstract(
	    uint16_t channel,
	    uint32_t sample_rate)
	    : channel_(channel), sample_rate_(sample_rate) {}

public: /* abstract */
	virtual ~sound_abstract() {}

public: /* abstract */
	virtual int8_t open() = 0;
	virtual int8_t close() = 0;
};

class sound_producer : public sound_abstract {
public: /* constructor */
	sound_producer(
	    uint16_t channel,
	    uint32_t sample_rate)
	    : sound_abstract(channel, sample_rate) {}

public: /* abstract */
	virtual std::vector<int16_t> produce() = 0;
};

class sound_consumer : public sound_abstract {
public: /* constructor */
	sound_consumer(
	    uint16_t channel,
	    uint32_t sample_rate)
	    : sound_abstract(channel, sample_rate) {}

public: /* abstract */
	virtual void consume(std::vector<int16_t> &pcm) = 0;
};