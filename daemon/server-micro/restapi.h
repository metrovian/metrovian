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

class restapi_singleton {
protected: /* microhttpd struct */
	struct MHD_Daemon *server_ = nullptr;

protected: /* handler */
	static MHD_Result handle_request(
	    void *cls,
	    struct MHD_Connection *connection,
	    const char *url,
	    const char *method,
	    const char *version,
	    const char *data,
	    size_t *size,
	    void **con_cls);

protected: /* parser */
	void parse(
	    struct MHD_Connection *connection,
	    const char *uri);

public: /* instance */
	static restapi_singleton &instance();

public: /* export */
	std::string update();

private: /* constraint */
	~restapi_singleton();
	restapi_singleton();
	restapi_singleton(const restapi_singleton &) = default;
	restapi_singleton &operator=(const restapi_singleton &) = default;
};