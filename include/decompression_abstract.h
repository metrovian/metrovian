#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include <atomic>
#include <condition_variable>
#include <map>
#include <mutex>
#include <queue>
#include <thread>
#include <sstream>
#include <string>
#include <stdint.h>
#include <spdlog/spdlog.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

class decompression_abstract {
protected: /* playback queue */
	std::thread producer_thread;
	std::thread consumer_thread;
	std::queue<std::vector<uint8_t>> queue;
	std::atomic<uint8_t> queue_state;
	std::mutex queue_mutex;
	std::condition_variable queue_cvar;

protected: /* playback memory */
	void push(std::vector<uint8_t> &payload);
	void pop(std::vector<uint8_t> &payload);
	void clear();

public: /* playback control */
	void decompress(const std::string &path);

public: /* constructor */
	decompression_abstract();

public: /* abstract */
	virtual ~decompression_abstract();

protected: /* abstract */
	virtual std::vector<uint8_t> decompression(const std::vector<uint8_t> &payload);
};