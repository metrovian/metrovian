#include "daemon/api/write/abstract.h"
#include "daemon/api/response.h"
#include "daemon/machine/automata.h"

MHD_Result write_play::parse(MHD_Connection *connection, std::string param) {
	if (automata_singleton::instance().play(param) != 0) {
		return response::empty(connection, MHD_HTTP_BAD_REQUEST);
	}

	return response::empty(connection, MHD_HTTP_OK);
}