#include "daemon/server-micro/parser/router.h"
#include "daemon/server-micro/parser/read/abstract.h"
#include "daemon/server-micro/parser/write/abstract.h"
#include "daemon/server-micro/context.h"
#include "daemon/server-micro/response.h"
#include "core/property.h"
#include "core/predefined.h"

router_singleton &router_singleton::instance() {
	static router_singleton instance_;
	return instance_;
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

MHD_Result router_singleton::upload(
    struct MHD_Connection *connection,
    const char *data,
    size_t *size,
    void **con_cls) {
	std::unordered_map<std::string, std::string> params;
	if (MHD_get_connection_values(
		connection,
		MHD_GET_ARGUMENT_KIND,
		&router_singleton::handle_query,
		&params) < 0) {
		return response::empty(connection, MHD_HTTP_BAD_REQUEST);
	}

	if (params.size() != 1) {
		return response::empty(connection, MHD_HTTP_BAD_REQUEST);
	}

	auto iter = params.find("action");
	if (iter == params.end()) {
		return response::empty(connection, MHD_HTTP_BAD_REQUEST);
	}

	params.erase(iter);
	if (iter->second.compare("upload") != 0) {
		return response::empty(connection, MHD_HTTP_BAD_REQUEST);
	}

	if (*con_cls == nullptr) {
		*con_cls = new std::string();
		return MHD_YES;
	}

	auto body = static_cast<std::string *>(*con_cls);
	if (*size != 0) {
		body->append((const char *)data, *size);
		*size = 0;
		return MHD_YES;
	}

	int code = MHD_HTTP_BAD_REQUEST;
	auto upload = nlohmann::ordered_json::parse(*body, nullptr, false);
	if (upload.is_discarded() == false) {
		if (upload.is_array() == true) {
			code = MHD_HTTP_OK;
			context_singleton::instance().set_presets(upload);
			spdlog::info("[api] [{}:{}] {}", LOG_PATH, __LINE__, __func__);
		}
	}

	delete body;
	*con_cls = nullptr;
	return response::empty(connection, code);
}

MHD_Result router_singleton::handle_query(
    void *cls,
    MHD_ValueKind,
    const char *key,
    const char *value) {
	auto *params = static_cast<std::unordered_map<std::string, std::string> *>(cls);
	(*params)[key] = (value != nullptr) ? std::string(value) : std::string("");
	return MHD_Result::MHD_YES;
}

router_singleton::router_singleton() {
	rmap_.insert(std::make_pair("mid", std::make_unique<read_mid>()));
	rmap_.insert(std::make_pair("mids", std::make_unique<read_mids>()));
	rmap_.insert(std::make_pair("notes", std::make_unique<read_notes>()));
	rmap_.insert(std::make_pair("progress", std::make_unique<read_progress>()));
	rmap_.insert(std::make_pair("state", std::make_unique<read_state>()));
	rmap_.insert(std::make_pair("waveform", std::make_unique<read_waveform>()));
	rmap_.insert(std::make_pair("waveforms", std::make_unique<read_waveforms>()));
	wmap_.insert(std::make_pair("play", std::make_unique<write_play>()));
	wmap_.insert(std::make_pair("reboot", std::make_unique<write_reboot>()));
	wmap_.insert(std::make_pair("volume", std::make_unique<write_volume>()));
	wmap_.insert(std::make_pair("waveform", std::make_unique<write_waveform>()));
	return;
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

	spdlog::info("[api] [{}:{}] {}:{}:{}", LOG_PATH, __LINE__, __func__, key, value);
	return iter->second->parse(connection, value);
}