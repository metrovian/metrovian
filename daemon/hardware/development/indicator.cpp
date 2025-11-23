#include "daemon/hardware/development/indicator.h"

void indicator_development::set(indicator::state state) {
	spdlog::info("[hardware] indicator: {}", static_cast<int32_t>(state));
	return;
}