#include "daemon/api/response.h"

MHD_Result response::empty(struct MHD_Connection *connection, uint32_t code) {
	struct MHD_Response *response =
	    MHD_create_response_from_buffer(
		0,
		nullptr,
		MHD_RESPMEM_PERSISTENT);

	MHD_Result ret = MHD_Result::MHD_NO;
	if (response != nullptr) {
		ret = queue_response(connection, response, code);
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
		ret = queue_response(connection, response, MHD_HTTP_OK);
		MHD_destroy_response(response);
	}

	return ret;
}

MHD_Result response::string(struct MHD_Connection *connection, std::string value) {
	struct MHD_Response *response =
	    MHD_create_response_from_buffer(
		value.size(),
		(void *)value.c_str(),
		MHD_RESPMEM_MUST_COPY);

	MHD_Result ret = MHD_Result::MHD_NO;
	if (response != nullptr) {
		ret = queue_response(connection, response, MHD_HTTP_OK);
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
		ret = queue_response(connection, response, MHD_HTTP_OK);
		MHD_destroy_response(response);
	}

	return ret;
}

MHD_Result response::queue_response(
    struct MHD_Connection *connection,
    struct MHD_Response *response,
    uint32_t code) {
	MHD_add_response_header(response, "Content-Type", "application/json");
	MHD_add_response_header(response, "Cache-Control", "no-store, no-cache, must-revalidate");
	MHD_add_response_header(response, "Pragma", "no-cache");
	MHD_add_response_header(response, "Expires", "0");
	MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
	return MHD_queue_response(connection, code, response);
}