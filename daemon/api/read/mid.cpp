#include "daemon/api/read/abstract.h"
#include "daemon/api/response.h"
#include "daemon/context.h"

MHD_Result read_mid::parse(MHD_Connection *connection) {
	return response::string(connection, context_api::read_mid());
}