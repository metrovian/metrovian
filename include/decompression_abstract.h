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
#include <libavutil/frame.h>
#include <libavutil/mem.h>
#include <libswresample/swresample.h>
#include <alsa/asoundlib.h>
}

class decompression_abstract {
protected: /* playback queue */
	std::thread producer_thread_;
	std::thread consumer_thread_;
	std::queue<std::vector<uint8_t>> queue_;
	std::atomic<uint8_t> queue_state_;
	std::mutex queue_mutex_;
	std::condition_variable queue_cvar_;

protected: /* playback queue */
	void push(std::vector<uint8_t> &payload);
	void pop(std::vector<uint8_t> &payload);
	void clear();

public: /* overload */
	void decompress(const std::string &path);

public: /* constructor */
	decompression_abstract();

public: /* abstract */
	virtual ~decompression_abstract();

protected: /* abstract */
	virtual int8_t open(AVCodecParameters *params) = 0;
	virtual int8_t close(AVCodecParameters *params) = 0;
	virtual std::vector<uint8_t> decompression(const std::vector<uint8_t> &payload, AVCodecParameters *params) = 0;
};

class decompression_general : public decompression_abstract {
protected: /* context */
	AVCodec *avcodec_ = nullptr;
	AVCodecContext *avcodec_ctx_ = nullptr;

protected: /* abstract */
	virtual int8_t open(AVCodecParameters *params) override final;
	virtual int8_t close(AVCodecParameters *params) override final;
	virtual std::vector<uint8_t> decompression(const std::vector<uint8_t> &payload, AVCodecParameters *params) override final;
};