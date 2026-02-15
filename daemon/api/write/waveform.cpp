#include "daemon/api/write/abstract.h"
#include "daemon/api/response.h"
#include "daemon/machine/automata.h"
#include "daemon/context.h"
#include "daemon/main.h"

MHD_Result write_waveform::parse(MHD_Connection *connection, std::string param) {
	if (std::stoi(param) >= 0) {
		context_api::write_preset(std::stoi(param));
		return response::empty(connection, MHD_HTTP_OK);
	}

	if ((context_api::read_state() & 0xF0) == 0x10) { // play
		automata_singleton::instance().panic();
		return response::empty(connection, MHD_HTTP_OK);
	} else if ((context_api::read_state() & 0x0F) == 0x03) { // performance
		machine_singleton::instance().panic();
		return response::empty(connection, MHD_HTTP_OK);
	}

	return response::empty(connection, MHD_HTTP_BAD_REQUEST);
}