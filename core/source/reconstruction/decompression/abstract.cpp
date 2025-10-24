#include "reconstruction/decompression/abstract.h"
#include "property.h"
#include "predefined.h"

void decompression_abstract::push(std::vector<uint8_t> &payload) {
	std::unique_lock<std::mutex> lock(queue_mutex_);
	queue_.push(payload);
	lock.unlock();
	queue_cvar_.notify_one();
	return;
}

void decompression_abstract::pop(std::vector<uint8_t> &payload) {
	std::unique_lock<std::mutex> lock(queue_mutex_);
	queue_cvar_.wait(lock, [&]() { return (queue_.empty() == 0) || (queue_state_.load() != 1); });
	if (queue_.empty() == 0) {
		payload = queue_.front();
		queue_.pop();
	}

	return;
}

void decompression_abstract::clear() {
	std::unique_lock<std::mutex> lock(queue_mutex_);
	while (queue_.empty() == 0) {
		queue_.pop();
	}

	return;
}

decompression_abstract::decompression_abstract() {
	queue_state_.store(0);
	queue_cvar_.notify_one();
}

decompression_abstract::~decompression_abstract() {
	queue_state_.store(0);
	queue_cvar_.notify_one();
	clear();
}