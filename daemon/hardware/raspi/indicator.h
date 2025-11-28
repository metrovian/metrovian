#pragma once
#include "daemon/hardware/abstract/indicator.h"

class indicator_raspi : public indicator_abstract {
protected: /* gpio flag */
	std::atomic<uint8_t> state_ = 0;

protected: /* gpio controller */
	struct gpiod_chip *chip_ = nullptr;
	struct gpiod_line *line_ = nullptr;

public: /* constructor */
	indicator_raspi();

public: /* destructor */
	virtual ~indicator_raspi() override final;

public: /* abstract */
	virtual void set(indicator::state state) override final;
};