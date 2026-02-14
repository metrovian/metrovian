#include "daemon/server-micro/parser/read/abstract.h"
#include "daemon/server-micro/context.h"
#include "daemon/server-micro/response.h"

MHD_Result read_notes::parse(MHD_Connection *connection) {
	return response::numeric(
	    connection,
	    static_cast<uint32_t>(context_singleton::instance().apiget_notes()));
}