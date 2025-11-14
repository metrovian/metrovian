#pragma once
#include "daemon/state/abstract.h"

class state_setup : public state_abstract {
public: /* abstract */
	virtual void enter();
	virtual void update();
};