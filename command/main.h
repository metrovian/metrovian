#pragma once
#include "command/abstract.h"

class interface_singleton {
public:
	static interface_singleton &instance();

public:
	interface_singleton(const interface_singleton &) = delete;
	interface_singleton(interface_singleton &&) = delete;
	interface_singleton &operator=(const interface_singleton &) = delete;
	interface_singleton &operator=(interface_singleton &&) = delete;
	int command_parse(int argc, char **argv);
	int command_setup(std::unique_ptr<command_abstract> command);

private:
	~interface_singleton() = default;
	interface_singleton();

private:
	CLI::App command_parser_;
	std::vector<std::unique_ptr<command_abstract>> commands_;
};