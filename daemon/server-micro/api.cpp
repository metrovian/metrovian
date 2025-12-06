#include "daemon/server-micro/api.h"
#include "daemon/server-micro/context.h"
#include "core/property.h"
#include "core/predefined.h"

MHD_Result api_singleton::handle_request(
    void *cls,
    struct MHD_Connection *connection,
    const char *,
    const char *method,
    const char *,
    const char *,
    size_t *,
    void **) {
	if (strncmp(method, "GET", 3) != 0) {
		return MHD_queue_response(
		    connection,
		    MHD_HTTP_METHOD_NOT_ALLOWED,
		    MHD_create_response_from_buffer(
			0,
			nullptr,
			MHD_RESPMEM_PERSISTENT));
	}

	return static_cast<api_singleton *>(cls)->parse(connection);
}

MHD_Result api_singleton::parse(struct MHD_Connection *connection) {
	const char *action =
	    MHD_lookup_connection_value(
		connection,
		MHD_GET_ARGUMENT_KIND,
		"action");

	if (strncmp(action, "read", 4) == 0) {
		const char *param = MHD_lookup_connection_value(
		    connection,
		    MHD_GET_ARGUMENT_KIND,
		    "state");

		if (param != nullptr) {
			uint8_t state = static_cast<uint8_t>(context_singleton::instance().update_state());
			std::string response = std::to_string(state);
			char *buf = (char *)malloc(response.length());
			memcpy(buf, response.c_str(), response.length());
			return MHD_queue_response(
			    connection,
			    MHD_HTTP_OK,
			    MHD_create_response_from_buffer(
				response.length(),
				(void *)buf,
				MHD_RESPMEM_MUST_FREE));
		}
	} else if (strncmp(action, "write", 5) == 0) {
		const char *waveform =
		    MHD_lookup_connection_value(
			connection,
			MHD_GET_ARGUMENT_KIND,
			"waveform");

		if (waveform != nullptr) {
			context_singleton::instance().transition(static_cast<machine::waveform>(std::stoi(waveform)));
			return MHD_queue_response(
			    connection,
			    MHD_HTTP_OK,
			    MHD_create_response_from_buffer(
				0,
				nullptr,
				MHD_RESPMEM_PERSISTENT));
		}
	}

	return MHD_queue_response(
	    connection,
	    MHD_HTTP_BAD_REQUEST,
	    MHD_create_response_from_buffer(
		0,
		nullptr,
		MHD_RESPMEM_PERSISTENT));
}

api_singleton &api_singleton::instance() {
	static api_singleton instance_;
	return instance_;
}

api_singleton::api_singleton() {
	LOG_ENTER();
	server_ = MHD_start_daemon(
	    MHD_USE_SELECT_INTERNALLY,
	    8080,
	    nullptr,
	    nullptr,
	    &api_singleton::handle_request,
	    this,
	    MHD_OPTION_END);

	if (server_ == nullptr) {
		LOG_CONDITION(MHD_start_daemon == nullptr);
		LOG_EXIT();
		return;
	}

	LOG_EXIT();
	return;
}

api_singleton::~api_singleton() {
	LOG_ENTER();
	if (server_ != nullptr) {
		MHD_stop_daemon(server_);
		server_ = nullptr;
	}

	LOG_EXIT();
	return;
}