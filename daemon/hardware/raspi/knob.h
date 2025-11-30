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
	struct gpiod_line *seg1_ = nullptr;
	struct gpiod_line *seg2_ = nullptr;
	struct gpiod_line *seg3_ = nullptr;
	struct gpiod_line *seg_a_ = nullptr;
	struct gpiod_line *seg_b_ = nullptr;
	struct gpiod_line *seg_c_ = nullptr;
	struct gpiod_line *seg_d_ = nullptr;
	struct gpiod_line *seg_e_ = nullptr;
	struct gpiod_line *seg_f_ = nullptr;
	struct gpiod_line *seg_g_ = nullptr;

protected: /* segment */
	void segment_set(uint8_t num, uint8_t count);

public: /* constructor */
	knob_raspi();

public: /* destructor */
	virtual ~knob_raspi() override final;

public: /* abstract */
	virtual void start() override final;
	virtual void stop() override final;

public: /* abstract */
	virtual uint16_t update() override final;
};