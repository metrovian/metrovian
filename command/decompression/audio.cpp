#include "decompression/audio.h"
#include "reconstruction/decompression/audio.h"
#include "predefined.h"

void command_audio::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("audio", "AUDIO decompression")->group("DECOMPRESSION");
	command->add_option("-i, --in", in_, "encoded audio")->required();
	command->add_option("-o, --out", out_, "decoded audio");
	command->callback([this]() { run(); });
	return;
}

void command_audio::run() {
	decompression_audio engine;
	out_.length() > 0 ? engine.decompress(in_, out_) : engine.decompress(in_);
	return;
}
