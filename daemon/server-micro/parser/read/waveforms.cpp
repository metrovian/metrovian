#include "daemon/server-micro/parser/read/waveforms.h"
#include "daemon/server-micro/context.h"
#include "daemon/server-micro/response.h"

MHD_Result read_waveforms::parse(MHD_Connection *connection) {
	return response::json(
	    connection,
	    context_singleton::instance().apiget_presets());
}