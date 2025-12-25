#pragma once
#include "daemon/state/abstract.h"

class state_none : public state_abstract {
public: /* abstract */
	virtual void enter() override final;
	virtual void update() override final;
};