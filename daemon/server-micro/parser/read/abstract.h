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

class read_abstract {
public:
	virtual ~read_abstract() {}
	virtual MHD_Result parse(struct MHD_Connection *connection) = 0;
};

#define DECL_READ(name)                                                \
	class read_##name final : public read_abstract {               \
	public:                                                        \
		MHD_Result parse(MHD_Connection *connection) override; \
	};

DECL_READ(mid);
DECL_READ(mids);
DECL_READ(notes);
DECL_READ(progress);
DECL_READ(state);
DECL_READ(waveform);
DECL_READ(waveforms);
#undef DECL_READ