#include "daemon/server-micro/parser/read/abstract.h"
#include "daemon/server-micro/response.h"
#include "daemon/context.h"

MHD_Result read_mid::parse(MHD_Connection *connection) {
	return response::string(connection, context_api::read_mid());
}