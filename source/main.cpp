#include "main.h"
#include "command/aes.h"
#include "command/rsa.h"
#include "command/avcodec.h"
#include "command/user.h"
#include "predefined.h"

int8_t interface_singleton::command_parse(int argc, char **argv) {
	CLI11_PARSE(command_parser_, argc, argv);
	return 0;
}

int8_t interface_singleton::command_setup(std::unique_ptr<command_abstract> command) {
	command->setup(&command_parser_);
	commands_.emplace_back(std::move(command));
	return 0;
}

void interface_singleton::init_command_parser() {
	auto formatter = std::make_shared<CLI::Formatter>();
	formatter->column_width(35);
	command_parser_.formatter(formatter);
	command_parser_.get_option("--help")->description("HELP command");
}

void interface_singleton::init_spdlog() {
	spdlog::set_default_logger(spdlog::stderr_color_mt("stderr"));
	spdlog::set_level(spdlog::level::trace);
}

interface_singleton &interface_singleton::instance() {
	static interface_singleton instance_;
	return instance_;
}

interface_singleton::interface_singleton() {
	init_command_parser();
	init_spdlog();
}

int32_t main(int argc, char **argv) {
	interface_singleton::instance().command_setup(std::make_unique<command_aes256_cbc>());
	interface_singleton::instance().command_setup(std::make_unique<command_aes256_ctr>());
	interface_singleton::instance().command_setup(std::make_unique<command_rsa>());
	interface_singleton::instance().command_setup(std::make_unique<command_rsa_attack>());
	interface_singleton::instance().command_setup(std::make_unique<command_avcodec>());
	interface_singleton::instance().command_setup(std::make_unique<command_user>());
	interface_singleton::instance().command_parse(argc, argv);
	return 0;
}