#include "decompression/avcodec.h"
#include "reconstruction/decompression/avcodec.h"
#include "predefined.h"

void command_avcodec::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("avcodec", "FFMPEG decompression")->group("DECOMPRESSION");
	command->add_option("-i, --in", in_, "encoded audio")->required();
	command->add_option("-o, --out", out_, "decoded audio");
	command->callback([this]() { run(); });
	return;
}

void command_avcodec::run() {
	decompression_avcodec engine;
	out_.length() > 0 ? engine.decompress(in_, out_) : engine.decompress(in_);
	return;
}
