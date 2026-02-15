#include "daemon/server-micro/parser/read/abstract.h"
#include "daemon/server-micro/response.h"
#include "daemon/context.h"

MHD_Result read_mids::parse(MHD_Connection *connection) {
	return response::json(connection, context_api::read_mids());
}