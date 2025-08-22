#include "interface_singleton.h"
#include "command_abstract.h"

int32_t main(int argc, char **argv) {
	interface_singleton::instance().command_setup(std::make_unique<command_aes256_cbc>());
	interface_singleton::instance().command_setup(std::make_unique<command_aes256_ctr>());
	interface_singleton::instance().command_setup(std::make_unique<command_avcodec>());
	interface_singleton::instance().command_parse(argc, argv);
	return 0;
}