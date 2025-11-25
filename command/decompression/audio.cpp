#include "command/decompression/audio.h"
#include "core/decompression/audio.h"
#include "core/predefined.h"

void command_audio::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("audio", "AUDIO decompression")->group("DECOMPRESSION");
	command->add_option("-i, --in", in_, "encoded audio")->required();
	command->add_option("-o, --out", out_, "decoded audio");
	command->callback([this]() { run(); });
	return;
}

void command_audio::run() {
	decompression_audio engine;
	handle_setup([&]() { engine.terminate(); });
	if (out_.length() > 0) {
		engine.decompress(in_, out_);
		return;
	}

	engine.decompress(in_);
	return;
}
