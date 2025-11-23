#pragma once
#include "daemon/state/abstract.h"

class state_synthesis : public state_abstract {
protected: /* complete flag */
	std::atomic<uint8_t> complete_ = 0;

public: /* abstract */
	virtual void enter() override final;
	virtual void update() override final;
};