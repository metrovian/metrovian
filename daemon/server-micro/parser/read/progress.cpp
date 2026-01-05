#include "daemon/server-micro/parser/read/progress.h"
#include "daemon/server-micro/context.h"
#include "daemon/server-micro/response.h"

MHD_Result read_progress::parse(MHD_Connection *connection) {
	return response::numeric(
	    connection,
	    static_cast<uint32_t>(context_singleton::instance().get_progress()));
}