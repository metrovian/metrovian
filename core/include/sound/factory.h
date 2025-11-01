#pragma once
#include "sound/abstract.h"

namespace sound {
enum class pipeline : uint8_t {
	common = 0,
	sync = 1,
};
}; // namespace sound

class sound_factory {
protected: /* sound */
	std::unique_ptr<sound_producer> producer_;
	std::unique_ptr<sound_consumer> consumer_;

protected: /* threadsafe queue */
	std::queue<std::vector<int16_t>> queue_;
	std::atomic<uint8_t> queue_state_;
	std::mutex queue_mutex_;
	std::condition_variable queue_cvar_;

protected: /* threadsafe queue */
	void push(std::vector<int16_t> &pcm);
	void pop(std::vector<int16_t> &pcm);
	void clear();

protected: /* thread */
	void thread_producer();
	void thread_consumer();
	void thread_sync();

public: /* export */
	void create();
	void run(sound::pipeline type);

public: /* constructor */
	sound_factory();

public: /* abstract */
	virtual ~sound_factory();

protected: /* abstract */
	virtual std::unique_ptr<sound_producer> create_producer() = 0;
	virtual std::unique_ptr<sound_consumer> create_consumer() = 0;
};