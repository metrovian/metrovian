#include "interface_singleton.h"

int8_t interface_singleton::command_parse(int argc, char **argv) {
	CLI11_PARSE(command_parser_, argc, argv);
	return 0;
}

int8_t interface_singleton::command_setup(std::unique_ptr<command_abstract> command) {
	command->setup(&command_parser_);
	commands_.emplace_back(std::move(command));
	return 0;
}

interface_singleton &interface_singleton::instance() {
	static interface_singleton instance_;
	return instance_;
}