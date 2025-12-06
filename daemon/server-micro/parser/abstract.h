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

class parser_abstract {
public: /* abstract */
	virtual MHD_Result parse(struct MHD_Connection *connection);
	virtual MHD_Result parse(struct MHD_Connection *connection, std::string param);
};