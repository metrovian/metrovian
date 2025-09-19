#include "command/avcodec.h"
#include "decompression/avcodec.h"
#include "predefined.h"

void command_avcodec::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("avcodec", "FFMPEG decompression")->group("DECOMPRESSION");
	command->add_option("-i, --in", in_, "encoded audio")->required();
	command->callback([this]() { run(); });
	return;
}

void command_avcodec::run() {
	decompression_avcodec engine;
	engine.decompress(in_);
	return;
}
