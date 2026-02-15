#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <microhttpd.h>

class read_abstract;
class write_abstract;

class router_singleton {
public:
	static router_singleton &instance();

public:
	router_singleton(const router_singleton &) = delete;
	router_singleton(router_singleton &&) = delete;
	router_singleton &operator=(const router_singleton &) = delete;
	router_singleton &operator=(router_singleton &&) = delete;
	MHD_Result parse(struct MHD_Connection *connection);
	MHD_Result upload(
	    struct MHD_Connection *connection,
	    const char *data,
	    size_t *size,
	    void **con_cls);

private:
	static MHD_Result handle_query(
	    void *cls,
	    enum MHD_ValueKind kind,
	    const char *key,
	    const char *value);

private:
	~router_singleton() = default;
	router_singleton();
	MHD_Result read(struct MHD_Connection *connection, std::string key);
	MHD_Result write(struct MHD_Connection *connection, std::string key, std::string value);

private:
	std::unordered_map<std::string, std::unique_ptr<read_abstract>> rmap_;
	std::unordered_map<std::string, std::unique_ptr<write_abstract>> wmap_;
};
