#include "daemon/server-micro/parser/write/waveform.h"
#include "daemon/server-micro/context.h"
#include "daemon/server-micro/response.h"
#include "daemon/main.h"

MHD_Result write_waveform::parse(MHD_Connection *connection, std::string param) {
	if (std::stoi(param) >= 0) {
		context_singleton::instance().apiset_preset(std::stoi(param));
		return response::empty(connection, MHD_HTTP_OK);
	}

	if (context_singleton::instance().apiget_state() ==
	    machine::state::performance) {
		machine_singleton::instance().panic();
		return response::empty(connection, MHD_HTTP_OK);
	}

	return response::empty(connection, MHD_HTTP_BAD_REQUEST);
}