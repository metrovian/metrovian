#include "daemon/hardware/development/indicator.h"
#include "core/predefined.h"

void indicator_development::set(indicator::state state) {
	LOG_VALUE(static_cast<int32_t>(state));
	return;
}