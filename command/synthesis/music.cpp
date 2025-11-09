#include "synthesis/music.h"
#include "synthesis/sin.h"
#include "synthesis/saw.h"
#include "synthesis/square.h"
#include "synthesis/unison.h"
#include "synthesis/hammond.h"
#include "predefined.h"

void command_music::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("music", "MUSIC synthesis")->group("SYNTHESIS");
	command->add_option("-w, --waveform", waveform_, "performance waveform")->required();
	command->add_option("-p, --params", params_, "parameters");
	command->callback([this]() { run(); });
	map_.insert(std::make_pair<std::string, music::waveform>("sin", music::waveform::sin));
	map_.insert(std::make_pair<std::string, music::waveform>("saw", music::waveform::saw));
	map_.insert(std::make_pair<std::string, music::waveform>("square", music::waveform::square));
	map_.insert(std::make_pair<std::string, music::waveform>("unison", music::waveform::unison));
	map_.insert(std::make_pair<std::string, music::waveform>("hammond", music::waveform::hammond));
	return;
}

void command_music::run() {
	synthesis_abstract *engine = nullptr;
	switch (params_.size()) {
	case 0: // clang-format off
		switch (map_[waveform_]) {
		case music::waveform::sin: engine = new synthesis_sin; break;
		case music::waveform::saw: engine = new synthesis_saw; break;
		case music::waveform::square: engine = new synthesis_square; break;
		case music::waveform::unison: engine = new synthesis_unison; break;
		case music::waveform::hammond: engine = new synthesis_hammond; break;
		default: break;
		} break;
	case 1:
		switch (map_[waveform_]) {
		case music::waveform::saw: engine = new synthesis_saw(params_[0]); break;
		case music::waveform::square: engine = new synthesis_square(params_[0]); break;
		default: break;
		} break;
	case 2: 
		switch (map_[waveform_]) {
		case music::waveform::unison: engine = new synthesis_unison(params_[0], params_[1]); break;
		default: break;
		} break;
	case 9:
		switch (map_[waveform_]) {
		case music::waveform::hammond: engine = new synthesis_hammond(params_); break;
		default: break;
		} break;
	default: // clang-format on
		break;
	}

	if (engine != nullptr) {
		handle_setup([&]() { engine->terminate(); });
		engine->synthesize();
		delete engine;
		engine = nullptr;
	}

	return;
}
