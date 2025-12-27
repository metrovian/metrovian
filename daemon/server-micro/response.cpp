#include "daemon/server-micro/response.h"

MHD_Result response::empty(struct MHD_Connection *connection, uint32_t code) {
	struct MHD_Response *response =
	    MHD_create_response_from_buffer(
		0,
		nullptr,
		MHD_RESPMEM_PERSISTENT);

	MHD_Result ret = MHD_Result::MHD_NO;
	if (response != nullptr) {
		ret = MHD_add_response_header(response, "Content-Type", "text/plain");
		ret = MHD_add_response_header(response, "Cache-Control", "no-store, no-cache, must-revalidate");
		ret = MHD_add_response_header(response, "Pragma", "no-cache");
		ret = MHD_add_response_header(response, "Expires", "0");
		ret = MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
		ret = MHD_queue_response(connection, code, response);
	}

	return ret;
}

MHD_Result response::numeric(struct MHD_Connection *connection, uint32_t value) {
	std::string str_value = std::to_string(value);
	struct MHD_Response *response =
	    MHD_create_response_from_buffer(
		str_value.size(),
		(void *)str_value.c_str(),
		MHD_RESPMEM_MUST_COPY);

	MHD_Result ret = MHD_Result::MHD_NO;
	if (response != nullptr) {
		ret = MHD_add_response_header(response, "Content-Type", "text/plain");
		ret = MHD_add_response_header(response, "Cache-Control", "no-store, no-cache, must-revalidate");
		ret = MHD_add_response_header(response, "Pragma", "no-cache");
		ret = MHD_add_response_header(response, "Expires", "0");
		ret = MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
		ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
		MHD_destroy_response(response);
	}

	return ret;
}

MHD_Result response::json(struct MHD_Connection *connection, const nlohmann::ordered_json &object) {
	std::string str_object = object.dump();
	struct MHD_Response *response =
	    MHD_create_response_from_buffer(
		str_object.size(),
		(void *)str_object.c_str(),
		MHD_RESPMEM_MUST_COPY);

	MHD_Result ret = MHD_Result::MHD_NO;
	if (response != nullptr) {
		MHD_add_response_header(response, "Content-Type", "application/json");
		MHD_add_response_header(response, "Cache-Control", "no-store, no-cache, must-revalidate");
		MHD_add_response_header(response, "Pragma", "no-cache");
		MHD_add_response_header(response, "Expires", "0");
		MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
		ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
		MHD_destroy_response(response);
	}

	return ret;
}