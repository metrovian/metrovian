#include "daemon/server-micro/parser/write/abstract.h"
#include "daemon/server-micro/automata.h"
#include "daemon/server-micro/response.h"

MHD_Result write_play::parse(MHD_Connection *connection, std::string param) {
	if (automata_singleton::instance().open(param) != 0) {
		return response::empty(connection, MHD_HTTP_BAD_REQUEST);
	}

	return response::empty(connection, MHD_HTTP_OK);
}