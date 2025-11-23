#pragma once
#include "daemon/hardware/abstract/knob.h"

class knob_development : public knob_abstract {
public: /* abstract */
	virtual void start() override final;
	virtual void stop() override final;

protected: /* abstract */
	virtual uint16_t read() override final;
};