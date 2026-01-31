#include "daemon/server-micro/parser/read/waveform.h"
#include "daemon/server-micro/context.h"
#include "daemon/server-micro/response.h"

MHD_Result read_waveform::parse(MHD_Connection *connection) {
	return response::json(
	    connection,
	    context_singleton::instance().apiget_presets());
}