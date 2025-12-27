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
#include <nlohmann/json.hpp>

class response {
public: /* export */
	static MHD_Result empty(struct MHD_Connection *connection, uint32_t code);
	static MHD_Result numeric(struct MHD_Connection *connection, uint32_t value);
	static MHD_Result json(struct MHD_Connection *connection, const nlohmann::ordered_json &object);
};