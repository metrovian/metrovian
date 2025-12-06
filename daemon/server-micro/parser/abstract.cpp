#include "daemon/server-micro/parser/abstract.h"

MHD_Result parser_abstract::parse(MHD_Connection *) {
	return MHD_Result::MHD_NO;
}

MHD_Result parser_abstract::parse(MHD_Connection *, std::string) {
	return MHD_Result::MHD_NO;
}