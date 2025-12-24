#include "daemon/server-micro/parser/write/waveform.h"
#include "daemon/server-micro/context.h"
#include "daemon/server-micro/response.h"

MHD_Result write_waveform::parse(MHD_Connection *connection, std::string param) {
	context_singleton::instance().set_preset(std::stoi(param));
	return response::empty(connection, MHD_HTTP_OK);
}