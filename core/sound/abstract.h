#pragma once
#include <stdint.h>
#include <vector>

class sound_abstract {
public:
	virtual ~sound_abstract() = default;
	sound_abstract(
	    uint16_t channel,
	    uint32_t sample_rate)
	    : channel_(channel), sample_rate_(sample_rate) {}

	virtual int open() = 0;
	virtual int close() = 0;

protected:
	uint16_t channel_ = 0;
	uint32_t sample_rate_ = 0;
};

class sound_producer : public sound_abstract {
public:
	sound_producer(
	    uint16_t channel,
	    uint32_t sample_rate)
	    : sound_abstract(channel, sample_rate) {}

	virtual std::vector<int16_t> produce() = 0;
};

class sound_consumer : public sound_abstract {
public:
	sound_consumer(
	    uint16_t channel,
	    uint32_t sample_rate)
	    : sound_abstract(channel, sample_rate) {}

	virtual void consume(std::vector<int16_t> &pcm) = 0;
};