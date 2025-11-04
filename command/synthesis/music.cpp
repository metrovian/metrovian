#include "synthesis/music.h"
#include "synthesis/sin.h"
#include "synthesis/saw.h"
#include "synthesis/square.h"
#include "predefined.h"

void command_music::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("music", "MUSIC synthesis")->group("SYNTHESIS");
	command->add_option("-w, --waveform", model_, "music waveform")->required();
	command->callback([this]() { run(); });
	map_.insert(std::make_pair<std::string, music::waveform>("sin", music::waveform::sin));
	map_.insert(std::make_pair<std::string, music::waveform>("saw", music::waveform::saw));
	map_.insert(std::make_pair<std::string, music::waveform>("square", music::waveform::square));
	return;
}

void command_music::run() {
	synthesis_abstract *engine = nullptr;
	// clang-format off
	switch (map_[model_]) {
	case music::waveform::sin: engine = new synthesis_sin; break;
	case music::waveform::saw: engine = new synthesis_saw; break;
	case music::waveform::square: engine = new synthesis_square; break;
	}
	// clang-format on
	if (engine != nullptr) {
		handle_setup([&]() { engine->terminate(); });
		engine->synthesize();
		delete engine;
		engine = nullptr;
	}

	return;
}
