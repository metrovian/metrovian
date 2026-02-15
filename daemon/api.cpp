#include "daemon/api.h"
#include "daemon/api/response.h"
#include "daemon/api/router.h"
#include "core/property.h"
#include "core/predefined.h"

api_singleton &api_singleton::instance() {
	static api_singleton instance_;
	return instance_;
}

MHD_Result api_singleton::handle_request(
    void *,
    struct MHD_Connection *connection,
    const char *,
    const char *method,
    const char *,
    const char *data,
    size_t *size,
    void **con_cls) {
	if (strncmp(method, "GET", 3) == 0) {
		return router_singleton::instance().parse(connection);
	} else if (strncmp(method, "POST", 4) == 0) {
		return router_singleton::instance().upload(connection, data, size, con_cls);
	}

	return response::empty(connection, MHD_HTTP_METHOD_NOT_ALLOWED);
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