#include "daemon/server-micro/parser/router.h"
#include "daemon/server-micro/response.h"
#include "daemon/server-micro/parser/read/state.h"
#include "daemon/server-micro/parser/read/waveform.h"
#include "daemon/server-micro/parser/write/waveform.h"

MHD_Result router_singleton::handle_query(
    void *cls,
    MHD_ValueKind,
    const char *key,
    const char *value) {
	auto *params = static_cast<std::unordered_map<std::string, std::string> *>(cls);
	(*params)[key] = (value != nullptr) ? std::string(value) : std::string("");
	return MHD_Result::MHD_YES;
}

MHD_Result router_singleton::read(MHD_Connection *connection, std::string key) {
	auto iter = rmap_.find(key);
	if (iter == rmap_.end()) {
		return response::empty(connection, MHD_HTTP_BAD_REQUEST);
	}

	return iter->second->parse(connection);
}

MHD_Result router_singleton::write(MHD_Connection *connection, std::string key, std::string value) {
	auto iter = wmap_.find(key);
	if (iter == wmap_.end()) {
		return response::empty(connection, MHD_HTTP_BAD_REQUEST);
	}

	return iter->second->parse(connection, value);
}

MHD_Result router_singleton::parse(struct MHD_Connection *connection) {
	std::unordered_map<std::string, std::string> params;
	if (MHD_get_connection_values(
		connection,
		MHD_GET_ARGUMENT_KIND,
		&router_singleton::handle_query,
		&params) < 0) {
		return response::empty(connection, MHD_HTTP_BAD_REQUEST);
	}

	if (params.size() != 2) {
		return response::empty(connection, MHD_HTTP_BAD_REQUEST);
	}

	auto iter = params.find("action");
	if (iter == params.end()) {
		return response::empty(connection, MHD_HTTP_BAD_REQUEST);
	}

	params.erase(iter);
	if (iter->second.compare("read") == 0) {
		return read(
		    connection,
		    params.begin()->first);
	} else if (iter->second.compare("write") == 0) {
		return write(
		    connection,
		    params.begin()->first,
		    params.begin()->second);
	}

	return response::empty(connection, MHD_HTTP_BAD_REQUEST);
}

router_singleton &router_singleton::instance() {
	static router_singleton instance_;
	return instance_;
}

void router_singleton::load_rmap() {
	rmap_.insert(std::make_pair("state", std::make_unique<read_state>()));
	rmap_.insert(std::make_pair("waveform", std::make_unique<read_waveform>()));
	return;
}

void router_singleton::load_wmap() {
	wmap_.insert(std::make_pair("waveform", std::make_unique<write_waveform>()));
	return;
}

router_singleton::router_singleton() {
	load_rmap();
	load_wmap();
}