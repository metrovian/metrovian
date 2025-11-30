#pragma once
#include "daemon/hardware/abstract/knob.h"

class segment_raspi {
protected: /* gpio flag */
	std::atomic<uint8_t> state_ = 0;
	std::atomic<uint8_t> knob_ = 0;

protected: /* gpio controller */
	struct gpiod_chip *chip_ = nullptr;
	struct gpiod_line *line1_ = nullptr;
	struct gpiod_line *line2_ = nullptr;
	struct gpiod_line *line3_ = nullptr;
	struct gpiod_line *line_a_ = nullptr;
	struct gpiod_line *line_b_ = nullptr;
	struct gpiod_line *line_c_ = nullptr;
	struct gpiod_line *line_d_ = nullptr;
	struct gpiod_line *line_e_ = nullptr;
	struct gpiod_line *line_f_ = nullptr;
	struct gpiod_line *line_g_ = nullptr;

public: /* constructor */
	segment_raspi();

public: /* destructor */
	virtual ~segment_raspi();

public: /* export */
	void set(uint8_t num);
};