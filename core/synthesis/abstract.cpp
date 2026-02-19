#include "core/synthesis/abstract.h"
#include "core/synthesis/oscillator.h"
#include "core/sound/player.h"
#include "core/sound/sequencer.h"
#include "core/property.h"
#include "core/predefined.h"

synthesis_abstract::synthesis_abstract() {
	create();
	callback_disconnect([&]() { terminate(); });
	callback_change([&](unsigned param, int value) {
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

void synthesis_abstract::synthesize() {
	synthesis(
	    CONFIG_UINT64("synthesis", "note-min"),
	    CONFIG_UINT64("synthesis", "note-max"),
	    CONFIG_UINT64("synthesis", "period"));
	return;
}

void synthesis_abstract::perform() {
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

void synthesis_abstract::callback_change(std::function<void(unsigned, int)> function) {
	dynamic_cast<sound_sequencer *>(producer_.get())->callback_change(function);
	return;
}

void synthesis_abstract::callback_synthesis(std::function<void(uint64_t)> function) {
	on_synthesis_ = function;
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

void synthesis_abstract::set_sample(uint64_t note, std::vector<int16_t> &pcm) {
	dynamic_cast<sound_sequencer *>(producer_.get())->set_sample(note, pcm);
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