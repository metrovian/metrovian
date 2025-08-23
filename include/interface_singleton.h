#pragma once
#include "command_abstract.h"

class interface_singleton {
protected: /* command parser */
	CLI::App command_parser_;
	std::vector<std::unique_ptr<command_abstract>> commands_;

public: /* command parser */
	int8_t command_parse(int argc, char **argv);
	int8_t command_setup(std::unique_ptr<command_abstract> command);

public: /* instance */
	static interface_singleton &instance();

private: /* constraint */
	interface_singleton();
	interface_singleton(const interface_singleton &) = default;
	interface_singleton &operator=(const interface_singleton &) = default;
};