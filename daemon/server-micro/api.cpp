#include "daemon/server-micro/api.h"
#include "daemon/server-micro/context.h"
#include "daemon/server-micro/response.h"
#include "daemon/server-micro/parser/router.h"
#include "core/property.h"
#include "core/predefined.h"

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
	}

	if (strncmp(method, "POST", 4) == 0) {
		if (*con_cls == nullptr) {
			*con_cls = new std::string();
			return MHD_YES;
		}

		auto body = static_cast<std::string *>(*con_cls);
		if (*size != 0) {
			body->append((const char *)data, *size);
			*size = 0;
			return MHD_YES;
		}

		int code = MHD_HTTP_BAD_REQUEST;
		auto upload = nlohmann::json::parse(*body, nullptr, false);
		if (upload.is_discarded() == false) {
			if (upload.is_array() == true) {
				code = MHD_HTTP_OK;
				context_singleton::instance().set_presets(upload);
			}
		}

		delete body;
		*con_cls = nullptr;
		return response::empty(connection, code);
	}

	return response::empty(connection, MHD_HTTP_METHOD_NOT_ALLOWED);
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