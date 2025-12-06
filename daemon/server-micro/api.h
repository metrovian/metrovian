#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <atomic>
#include <thread>
#include <unistd.h>
#include <spdlog/spdlog.h>
#include <microhttpd.h>

class api_singleton {
protected: /* microhttpd struct */
	struct MHD_Daemon *server_ = nullptr;

protected: /* handler */
	static MHD_Result handle_request(
	    void *cls,
	    struct MHD_Connection *connection,
	    const char *uri,
	    const char *method,
	    const char *version,
	    const char *data,
	    size_t *size,
	    void **con_cls);

public: /* instance */
	static api_singleton &instance();

private: /* constraint */
	~api_singleton();
	api_singleton();
	api_singleton(const api_singleton &) = default;
	api_singleton &operator=(const api_singleton &) = default;
};