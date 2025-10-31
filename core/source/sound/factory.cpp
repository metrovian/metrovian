#include "sound/factory.h"
#include "predefined.h"

void sound_factory::push(std::vector<int16_t> &payload) {
	std::unique_lock<std::mutex> lock(queue_mutex_);
	queue_.push(payload);
	lock.unlock();
	queue_cvar_.notify_one();
	return;
}

void sound_factory::pop(std::vector<int16_t> &payload) {
	std::unique_lock<std::mutex> lock(queue_mutex_);
	queue_cvar_.wait(lock, [&]() { return (queue_.empty() == 0) || (queue_state_.load() != 1); });
	if (queue_.empty() == 0) {
		payload = queue_.front();
		queue_.pop();
	}

	return;
}

void sound_factory::clear() {
	std::unique_lock<std::mutex> lock(queue_mutex_);
	while (queue_.empty() == 0) {
		queue_.pop();
	}

	return;
}

void sound_factory::thread_producer() {
	producer_->open();
	queue_state_.store(1);
	while (queue_state_.load() == 1) {
		std::vector<int16_t> pcm = producer_->produce();
		push(pcm);
	}

	queue_state_.store(2);
	queue_cvar_.notify_one();
	producer_->close();
}

void sound_factory::thread_consumer() {
	consumer_->open();
	while (queue_state_.load() != 0) {
		std::vector<int16_t> pcm;
		pop(pcm);
		if (pcm.empty()) {
			break;
		}

		consumer_->consume(pcm);
	}

	consumer_->close();
}

void sound_factory::thread_sync() {
	producer_->open();
	consumer_->open();
	queue_state_.store(1);
	while (queue_state_.load() != 0) {
		std::vector<int16_t> pcm = producer_->produce();
		consumer_->consume(pcm);
	}

	producer_->close();
	consumer_->close();
}

void sound_factory::create() {
	producer_ = create_producer();
	consumer_ = create_consumer();
	return;
}

void sound_factory::run(sound::pipeline type) {
	switch (type) {
	case sound::pipeline::common:
		std::jthread(&sound_factory::thread_producer, this);
		std::jthread(&sound_factory::thread_consumer, this);
		break;
	case sound::pipeline::sync:
		std::jthread(&sound_factory::thread_sync, this);
		break;
	default:
		LOG_ARGUMENT(type);
		break;
	}

	return;
}

sound_factory::sound_factory() {
	queue_state_.store(0);
	queue_cvar_.notify_one();
}

sound_factory::~sound_factory() {
	queue_state_.store(0);
	queue_cvar_.notify_one();
	clear();
}