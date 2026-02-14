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
#include <sys/reboot.h>
#include <linux/reboot.h>
#include <spdlog/spdlog.h>
#include <microhttpd.h>

class write_abstract {
public:
	virtual ~write_abstract() {}
	virtual MHD_Result parse(struct MHD_Connection *connection, std::string param) = 0;
};

#define DECL_WRITE(name)                                                                         \
	class write_##name final : public write_abstract {                                       \
	public:                                                                                  \
		MHD_Result parse(struct MHD_Connection *connection, std::string param) override; \
	};

DECL_WRITE(play);
DECL_WRITE(reboot);
DECL_WRITE(volume);
DECL_WRITE(waveform);
#undef DECL_WRITE