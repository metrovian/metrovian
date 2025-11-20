#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <vector>
#include <map>
#include <memory>

namespace indicator {
enum class state : uint8_t {
	off = 0,
	on = 1,
	blink_slow = 2,
	blink_fast = 3,
};
}; // namespace indicator

class indicator_abstract {
protected: /* abstract */
	virtual void set(indicator::state state) = 0;
};