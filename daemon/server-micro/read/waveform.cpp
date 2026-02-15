#include "daemon/server-micro/read/abstract.h"
#include "daemon/server-micro/response.h"
#include "daemon/context.h"

MHD_Result read_waveform::parse(MHD_Connection *connection) {
	return response::numeric(connection, context_api::read_id());
}