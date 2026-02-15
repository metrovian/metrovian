#pragma once
#include <microhttpd.h>

class read_abstract {
public:
	virtual ~read_abstract() = default;
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