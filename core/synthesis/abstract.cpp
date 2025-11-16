#include "core/synthesis/abstract.h"
#include "core/property.h"
#include "core/predefined.h"

void synthesis_abstract::resize(uint64_t note) {
	dynamic_cast<sound_sequencer *>(producer_.get())->resize(note);
	return;
}

void synthesis_abstract::resample(uint64_t note, std::vector<int16_t> &pcm) {
	dynamic_cast<sound_sequencer *>(producer_.get())->resample(note, pcm);
	return;
}

void synthesis_abstract::callback_disconnect(std::function<void(void)> function) {
	dynamic_cast<sound_sequencer *>(producer_.get())->callback_disconnect(function);
	return;
}

void synthesis_abstract::callback_change(std::function<void(void)> function) {
	dynamic_cast<sound_sequencer *>(producer_.get())->callback_change(function);
	return;
}

int8_t synthesis_abstract::synthesize() {
	create();
	synthesis(
	    CONFIG_UINT64("synthesis", "note-min"),
	    CONFIG_UINT64("synthesis", "note-max"),
	    CONFIG_UINT64("synthesis", "period"));

	callback_disconnect([&]() { terminate(); });
	return 0;
}

int8_t synthesis_abstract::perform() {
	run(sound::pipeline::sync);
	return 0;
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