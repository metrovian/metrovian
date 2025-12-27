#include "command/synthesis/music.h"
#include "core/synthesis/add.h"
#include "core/predefined.h"

void command_music::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("music", "MUSIC synthesis")->group("SYNTHESIS");
	command->add_option("-m, --method", method_, "synthesis method")->required();
	command->add_option("-i, --in", in_, "preset json")->required();
	command->callback([this]() { run(); });
	map_.insert(std::make_pair<std::string, music::method>("add", music::method::add));
	map_.insert(std::make_pair<std::string, music::method>("fm", music::method::fm));
	return;
}

void command_music::run() {
	nlohmann::ordered_json preset;
	std::ifstream ifs(in_);
	if (ifs.is_open() == true) {
		ifs >> preset;
		ifs.close();
	}

	// clang-format off
	synthesis_abstract *engine = nullptr;
	switch (map_[method_]) {
	case music::method::add: engine = new synthesis_add(preset); break;
	case music::method::fm: break;
	default: return;
	}
	// clang-format on
	if (engine != nullptr) {
		handle_setup([&]() { engine->terminate(); });
		engine->synthesize();
		engine->perform();
		delete engine;
		engine = nullptr;
	}

	return;
}