#include "daemon/api/read/abstract.h"
#include "daemon/api/response.h"
#include "daemon/context.h"

MHD_Result read_waveforms::parse(MHD_Connection *connection) {
	return response::json(connection, context_api::read_presets());
}