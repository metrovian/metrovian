#pragma once
#include "daemon/state/abstract.h"

class state_synthesis : public state_abstract {
public: /* abstract */
	virtual void enter();
	virtual void exit();
	virtual void update();
};