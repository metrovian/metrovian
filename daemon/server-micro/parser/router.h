#pragma once
#include "daemon/server-micro/parser/abstract.h"

class router_singleton {
protected: /* router map */
	std::unordered_map<std::string, std::unique_ptr<parser_abstract>> rmap_;
	std::unordered_map<std::string, std::unique_ptr<parser_abstract>> wmap_;

protected: /* handler */
	static MHD_Result handle_query(
	    void *cls,
	    enum MHD_ValueKind kind,
	    const char *key,
	    const char *value);

protected: /* router action */
	MHD_Result read(struct MHD_Connection *connection, std::string key);
	MHD_Result write(struct MHD_Connection *connection, std::string key, std::string value);

public: /* export */
	MHD_Result parse(struct MHD_Connection *connection);
	MHD_Result upload(
	    struct MHD_Connection *connection,
	    const char *data,
	    size_t *size,
	    void **con_cls);

public: /* instance */
	static router_singleton &instance();

private: /* load */
	void load_rmap();
	void load_wmap();

private: /* constraint */
	router_singleton();
	router_singleton(const router_singleton &) = default;
	router_singleton &operator=(const router_singleton &) = default;
};