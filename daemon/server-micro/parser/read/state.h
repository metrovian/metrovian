#pragma once
#include "daemon/server-micro/parser/abstract.h"

class read_state : public parser_abstract {
public: /* abstract */
	virtual MHD_Result parse(struct MHD_Connection *connection) override final;
};