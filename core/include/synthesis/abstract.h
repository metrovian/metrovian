#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <map>
#include <mutex>
#include <cmath>
#include <string>
#include <vector>
#include <functional>
#include <spdlog/spdlog.h>

extern "C" {
#include <alsa/asoundlib.h>
}

namespace synthesis {
struct note {
	std::vector<int16_t> sample_;
	uint8_t active_ = 0;
	uint8_t vel_ = 0;
	uint64_t pos_ = 0;
};

class muxer {
protected: /* parameter */
	std::atomic<uint8_t> state_;
	std::mutex mutex_;

protected: /* control */
	std::vector<synthesis::note> key_;
	uint8_t sustain_ = 0;

public: /* setter */
	int8_t keysize(uint64_t note);
	int8_t keysample(std::vector<int16_t> &pcm, uint64_t note);

public: /* control */
	int8_t keydown(uint8_t vel, uint64_t note);
	int8_t keyrel(uint8_t vel, uint64_t note);
	int8_t sustain(uint8_t down);

public: /* output */
	std::vector<int16_t> mux(uint64_t len);
};

class sequencer {
protected: /* parameter */
	synthesis::muxer *muxer_ = nullptr;
	std::atomic<uint8_t> state_;

public: /* control */
	int8_t open(synthesis::muxer *ptr);
	int8_t close();
};

class player {
protected: /* parameter */
	synthesis::muxer *muxer_ = nullptr;
	std::atomic<uint8_t> state_;

public: /* control */
	int8_t open(synthesis::muxer *ptr);
	int8_t close();
};
}; // namespace synthesis

class synthesis_abstract {
protected: /* manager */
	synthesis::muxer muxer_;
	synthesis::sequencer sequencer_;
	synthesis::player player_;

public: /* overload */
	int8_t synthesize();
	int8_t synthesize(std::string score);

public: /* abstract */
	virtual ~synthesis_abstract() {}

protected: /* abstract */
	virtual void synthesis(uint64_t note, uint64_t period) = 0;
};