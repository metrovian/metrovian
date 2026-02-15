#pragma once
#include <microhttpd.h>

class api_singleton {
public:
	static api_singleton &instance();

public:
	api_singleton(const api_singleton &) = delete;
	api_singleton(api_singleton &&) = delete;
	api_singleton &operator=(const api_singleton &) = delete;
	api_singleton &operator=(api_singleton &&) = delete;

private:
	static MHD_Result handle_request(
	    void *cls,
	    struct MHD_Connection *connection,
	    const char *uri,
	    const char *method,
	    const char *version,
	    const char *data,
	    size_t *size,
	    void **con_cls);

private:
	~api_singleton();
	api_singleton();

private:
	struct MHD_Daemon *server_ = nullptr;
};