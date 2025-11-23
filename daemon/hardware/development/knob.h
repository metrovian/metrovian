#pragma once
#include "daemon/hardware/abstract/knob.h"

class knob_development : public knob_abstract {
protected: /* terminal */
	std::atomic<uint8_t> state_ = 0;
	std::atomic<uint16_t> value_ = 0;
	termios terminal_;

public: /* abstract */
	virtual void start() override final;
	virtual void stop() override final;

public: /* abstract */
	virtual uint16_t update() override final;
};