#pragma once
#include "daemon/server-micro/parser/abstract.h"

class write_reboot : public parser_abstract {
public: /* abstract */
	virtual MHD_Result parse(struct MHD_Connection *connection, std::string param) override final;
};