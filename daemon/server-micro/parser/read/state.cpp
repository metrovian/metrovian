#include "daemon/server-micro/parser/read/abstract.h"
#include "daemon/server-micro/response.h"
#include "daemon/context.h"

MHD_Result read_state::parse(MHD_Connection *connection) {
	return response::numeric(connection, static_cast<uint32_t>(context_api::read_state()));
}