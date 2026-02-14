#include "daemon/server-micro/parser/read/abstract.h"
#include "daemon/server-micro/automata.h"
#include "daemon/server-micro/response.h"

MHD_Result read_mids::parse(MHD_Connection *connection) {
	return response::json(
	    connection,
	    automata_singleton::instance().apiget_mids());
}