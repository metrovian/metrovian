#include "daemon/server-micro/restapi.h"
#include "core/property.h"
#include "core/predefined.h"

MHD_Result restapi_singleton::handle_request(
    void *cls,
    struct MHD_Connection *connection,
    const char *url,
    const char *method,
    const char *version,
    const char *data,
    size_t *size,
    void **con_cls) {
	return MHD_Result::MHD_YES;
}

void restapi_singleton::parse(
    struct MHD_Connection *connection,
    const char *uri) {
}

restapi_singleton &restapi_singleton::instance() {
	static restapi_singleton instance_;
	return instance_;
}

std::string restapi_singleton::update() {
	return std::string();
}

restapi_singleton::restapi_singleton() {
	LOG_ENTER();
	server_ = MHD_start_daemon(
	    MHD_USE_SELECT_INTERNALLY,
	    8080,
	    nullptr,
	    nullptr,
	    &restapi_singleton::handle_request,
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

restapi_singleton::~restapi_singleton() {
	LOG_ENTER();
	if (server_ != nullptr) {
		MHD_stop_daemon(server_);
		server_ = nullptr;
	}

	LOG_EXIT();
	return;
}