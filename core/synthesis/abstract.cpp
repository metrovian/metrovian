#include "core/synthesis/abstract.h"
#include "core/synthesis/oscillator.h"
#include "core/sound/player.h"
#include "core/sound/sequencer.h"
#include "core/property.h"
#include "core/predefined.h"

synthesis_abstract::synthesis_abstract() {
	create();
	callback_disconnect([&]() { terminate(); });
	callback_change([&](uint32_t param, int32_t value) {
		switch (param) {
		case 0x07:
			set_scale(value);
			return;
		case 0x79:
			terminate();
			return;
		default:
			return;
		}
	});
}

synthesis_abstract::synthesis_abstract(const nlohmann::ordered_json &preset)
    : synthesis_abstract() {
	set_envelope(preset);
	return;
}

void synthesis_abstract::setup() {
	setup(CONFIG_UINT64("synthesis", "note-max"));
	return;
}

void synthesis_abstract::synthesis() {
	run(sound::pipeline::sync);
	return;
}

void synthesis_abstract::panic() {
	terminate();
	return;
}

void synthesis_abstract::callback_disconnect(std::function<void(void)> function) {
	dynamic_cast<sound_sequencer *>(producer_.get())->callback_disconnect(function);
	return;
}

void synthesis_abstract::callback_change(std::function<void(uint32_t, int32_t)> function) {
	dynamic_cast<sound_sequencer *>(producer_.get())->callback_change(function);
	return;
}

void synthesis_abstract::set_envelope(const nlohmann::ordered_json &preset) {
	auto iter = preset.find("envelope");
	if (iter != preset.end()) {
		if (iter->is_object() == true) {
			dynamic_cast<sound_sequencer *>(producer_.get())
			    ->set_envelope(
				iter->value("sustain", 1.000E+0),
				iter->value("attack", 0.000E+0),
				iter->value("decay", 0.000E+0),
				iter->value("release", 0.000E+0));
		}
	}

	return;
}

void synthesis_abstract::set_scale(uint16_t volume) {
	dynamic_cast<sound_sequencer *>(producer_.get())->set_scale(volume);
	return;
}

void synthesis_abstract::set_size(uint64_t note) {
	dynamic_cast<sound_sequencer *>(producer_.get())->set_size(note);
	return;
}

void synthesis_abstract::set_synthesis(std::function<int16_t(uint64_t, uint64_t)> function) {
	dynamic_cast<sound_sequencer *>(producer_.get())->callback_synthesis(function);
	return;
}

std::unique_ptr<sound_producer> synthesis_abstract::create_producer() {
	return std::make_unique<sound_sequencer>(
	    CONFIG_UINT32("synthesis", "channel"),
	    CONFIG_UINT32("synthesis", "sample-rate"));
}

std::unique_ptr<sound_consumer> synthesis_abstract::create_consumer() {
	return std::make_unique<sound_player>(
	    CONFIG_UINT32("synthesis", "channel"),
	    CONFIG_UINT32("synthesis", "sample-rate"));
}
