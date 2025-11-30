#pragma once
#include "daemon/hardware/abstract/knob.h"

class knob_raspi : public knob_abstract {
protected: /* gpio flag */
	std::atomic<uint8_t> state_ = 0;
	std::atomic<uint16_t> value_ = 0;

protected: /* gpio controller */
	struct gpiod_chip *chip_ = nullptr;
	struct gpiod_line *line1_ = nullptr;
	struct gpiod_line *line2_ = nullptr;
	struct gpiod_line *line3_ = nullptr;

public: /* destructor */
	virtual ~knob_raspi() override final;

public: /* abstract */
	virtual void start() override final;
	virtual void stop() override final;

public: /* abstract */
	virtual uint16_t update() override final;
};