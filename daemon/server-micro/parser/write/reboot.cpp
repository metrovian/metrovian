#include "daemon/server-micro/parser/write/reboot.h"
#include "daemon/server-micro/context.h"
#include "daemon/server-micro/response.h"

MHD_Result write_reboot::parse(MHD_Connection *connection, std::string param) {
	// clang-format off
	int action = std::stoi(param);
	int cmd = 0;
	switch (action) {
        case 0: cmd = LINUX_REBOOT_CMD_POWER_OFF; break;
        case 1: cmd = LINUX_REBOOT_CMD_RESTART; break;
        default: return response::empty(connection, MHD_HTTP_BAD_REQUEST);
        }
	// clang-format on
	if (reboot(cmd) != 0) {
		return response::empty(connection, MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	return response::empty(connection, MHD_HTTP_OK);
}