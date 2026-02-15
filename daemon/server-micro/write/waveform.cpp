#include "daemon/server-micro/write/abstract.h"
#include "daemon/server-micro/response.h"
#include "daemon/context.h"
#include "daemon/main.h"

MHD_Result write_waveform::parse(MHD_Connection *connection, std::string param) {
	if (std::stoi(param) >= 0) {
		context_api::write_preset(std::stoi(param));
		return response::empty(connection, MHD_HTTP_OK);
	}

	if (context_api::read_state() == static_cast<uint8_t>(machine::state::performance)) {
		machine_singleton::instance().panic();
		return response::empty(connection, MHD_HTTP_OK);
	}

	return response::empty(connection, MHD_HTTP_BAD_REQUEST);
}