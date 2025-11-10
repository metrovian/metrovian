#include "core/sound/factory.h"
#include "core/predefined.h"

void sound_factory::push(std::vector<int16_t> &pcm) {
	std::unique_lock<std::mutex> lock(queue_mutex_);
	queue_.push(pcm);
	lock.unlock();
	queue_cvar_.notify_one();
	return;
}

void sound_factory::pop(std::vector<int16_t> &pcm) {
	std::unique_lock<std::mutex> lock(queue_mutex_);
	queue_cvar_.wait(lock, [&]() { return (queue_.empty() == 0) || (queue_state_.load() != 1); });
	if (queue_.empty() == 0) {
		pcm = queue_.front();
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
	if (producer_->open() != 0) {
		return;
	}

	queue_state_.store(1);
	while (queue_state_.load() == 1) {
		std::vector<int16_t> pcm = producer_->produce();
		if (pcm.empty()) {
			break;
		}

		push(pcm);
	}

	queue_state_.store(2);
	queue_cvar_.notify_one();
	producer_->close();
	return;
}

void sound_factory::thread_consumer() {
	if (consumer_->open() != 0) {
		queue_state_.store(0);
		queue_cvar_.notify_one();
		return;
	}

	while (queue_state_.load() != 0) {
		std::vector<int16_t> pcm;
		pop(pcm);
		if (pcm.empty()) {
			break;
		}

		consumer_->consume(pcm);
	}

	queue_state_.store(0);
	queue_cvar_.notify_one();
	consumer_->close();
	return;
}

void sound_factory::thread_sync() {
	if (producer_->open() != 0) {
		return;
	}

	if (consumer_->open() != 0) {
		producer_->close();
		return;
	}

	queue_state_.store(1);
	while (queue_state_.load() != 0) {
		std::vector<int16_t> pcm = producer_->produce();
		consumer_->consume(pcm);
	}

	producer_->close();
	consumer_->close();
	return;
}

void sound_factory::run(sound::pipeline type) {
	std::vector<std::jthread> trds;
	switch (type) {
	case sound::pipeline::common:
		trds.push_back(std::jthread(&sound_factory::thread_producer, this));
		trds.push_back(std::jthread(&sound_factory::thread_consumer, this));
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

void sound_factory::terminate() {
	queue_state_.store(0);
	queue_cvar_.notify_one();
	return;
}

void sound_factory::create() {
	producer_ = create_producer();
	consumer_ = create_consumer();
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