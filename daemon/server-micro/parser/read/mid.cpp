#include "daemon/server-micro/parser/read/mid.h"
#include "daemon/server-micro/automata.h"
#include "daemon/server-micro/response.h"

MHD_Result read_mid::parse(MHD_Connection *connection) {
	return response::string(
	    connection,
	    automata_singleton::instance().apiget_mid());
}