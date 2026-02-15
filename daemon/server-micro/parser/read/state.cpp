#include "daemon/server-micro/parser/read/abstract.h"
#include "daemon/server-micro/automata.h"
#include "daemon/server-micro/response.h"
#include "daemon/context.h"

MHD_Result read_state::parse(MHD_Connection *connection) {
	uint32_t state = static_cast<uint32_t>(context_api::read_state());
	if (state == static_cast<uint32_t>(machine::state::performance)) {
		state += static_cast<uint32_t>(automata_singleton::instance().apiget_state());
	}

	return response::numeric(connection, state);
}