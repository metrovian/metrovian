#include "command/main.h"
#include "command/decompression/abstract.h"
#include "command/operation/abstract.h"
#include "command/optimization/abstract.h"
#include "core/predefined.h"

interface_singleton &interface_singleton::instance() {
	static interface_singleton instance_;
	return instance_;
}

interface_singleton::interface_singleton() {
	auto formatter = std::make_shared<CLI::Formatter>();
	formatter->column_width(40);
	command_parser_.formatter(formatter);
	command_parser_.get_option("--help")->description("");
	command_parser_.set_version_flag("--version, -v", std::string(__DATE__ " " __TIME__))->description("");
	std::cout << std::scientific << std::showpos << std::setprecision(6);
	spdlog::set_default_logger(spdlog::stderr_color_mt("stderr"));
	spdlog::set_level(spdlog::level::trace);
}

int interface_singleton::command_parse(int argc, char **argv) {
	CLI11_PARSE(command_parser_, argc, argv);
	return 0;
}

int interface_singleton::command_setup(std::unique_ptr<command_abstract> command) {
	command->setup(&command_parser_);
	commands_.emplace_back(std::move(command));
	return 0;
}

int main(int argc, char **argv) {
	interface_singleton::instance().command_setup(std::make_unique<command_audio>());
	interface_singleton::instance().command_setup(std::make_unique<command_corr>());
	interface_singleton::instance().command_setup(std::make_unique<command_desa>());
	interface_singleton::instance().command_setup(std::make_unique<command_polynomial>());
	interface_singleton::instance().command_setup(std::make_unique<command_rational>());
	interface_singleton::instance().command_setup(std::make_unique<command_sinusoidal>());
	interface_singleton::instance().command_setup(std::make_unique<command_transcendental>());
	interface_singleton::instance().command_setup(std::make_unique<command_user>());
	interface_singleton::instance().command_parse(argc, argv);
	return 0;
}