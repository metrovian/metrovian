#pragma once
#include <string>
#include <microhttpd.h>
#include <nlohmann/json.hpp>

class response {
public:
	static MHD_Result empty(struct MHD_Connection *connection, uint32_t code);
	static MHD_Result numeric(struct MHD_Connection *connection, uint32_t value);
	static MHD_Result string(struct MHD_Connection *connection, std::string value);
	static MHD_Result json(struct MHD_Connection *connection, const nlohmann::ordered_json &object);

private:
	static MHD_Result queue_response(
	    struct MHD_Connection *connection,
	    struct MHD_Response *response,
	    uint32_t code);
};