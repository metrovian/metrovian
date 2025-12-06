#pragma once
#include "daemon/server-micro/parser/abstract.h"

class write_waveform : public parser_abstract {
public: /* abstract */
	virtual MHD_Result parse(struct MHD_Connection *connection, std::string param) override final;
};