#include "main.h"
#include "command/optimization/polynomial.h"
#include "command/optimization/rational.h"
#include "command/optimization/sinusoidal.h"
#include "command/optimization/transcendental.h"
#include "command/optimization/user.h"
#include "command/decryption/aes.h"
#include "command/decryption/rsa.h"
#include "command/decryption/ecdh.h"
#include "command/decompression/avcodec.h"
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

void interface_singleton::load_command_parser() {
	auto formatter = std::make_shared<CLI::Formatter>();
	formatter->column_width(40);
	command_parser_.formatter(formatter);
	command_parser_.get_option("--help")->description("HELP command");
}

void interface_singleton::load_stdout() {
	std::cout << std::scientific << std::showpos << std::setprecision(6);
}

void interface_singleton::load_stderr() {
	spdlog::set_default_logger(spdlog::stderr_color_mt("stderr"));
	spdlog::set_level(spdlog::level::trace);
}

interface_singleton &interface_singleton::instance() {
	static interface_singleton instance_;
	return instance_;
}

interface_singleton::interface_singleton() {
	load_command_parser();
	load_stdout();
	load_stderr();
}

int32_t main(int argc, char **argv) {
	interface_singleton::instance().command_setup(std::make_unique<command_polynomial>());
	interface_singleton::instance().command_setup(std::make_unique<command_rational>());
	interface_singleton::instance().command_setup(std::make_unique<command_sinusoidal>());
	interface_singleton::instance().command_setup(std::make_unique<command_transcendental>());
	interface_singleton::instance().command_setup(std::make_unique<command_user>());
	interface_singleton::instance().command_setup(std::make_unique<command_aes256_cbc>());
	interface_singleton::instance().command_setup(std::make_unique<command_aes256_ctr>());
	interface_singleton::instance().command_setup(std::make_unique<command_rsa>());
	interface_singleton::instance().command_setup(std::make_unique<command_ecdh>());
	interface_singleton::instance().command_setup(std::make_unique<command_avcodec>());
	interface_singleton::instance().command_parse(argc, argv);
	return 0;
}