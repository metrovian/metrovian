#pragma once
#include "core/sound/abstract.h"

namespace sound {
enum class pipeline : uint8_t {
	common = 0,
	sync = 1,
};
}; // namespace sound

class sound_factory {
public:
	virtual ~sound_factory();
	sound_factory();
	void run(sound::pipeline type);
	void terminate();
	void create();

protected:
	virtual std::unique_ptr<sound_producer> create_producer() = 0;
	virtual std::unique_ptr<sound_consumer> create_consumer() = 0;

private:
	void push(std::vector<int16_t> &pcm);
	void pop(std::vector<int16_t> &pcm);
	void clear();
	void thread_producer();
	void thread_consumer();
	void thread_sync();

protected:
	std::unique_ptr<sound_producer> producer_;
	std::unique_ptr<sound_consumer> consumer_;

private:
	std::queue<std::vector<int16_t>> queue_;
	std::atomic<uint8_t> queue_state_;
	std::mutex queue_mutex_;
	std::condition_variable queue_cvar_;
};