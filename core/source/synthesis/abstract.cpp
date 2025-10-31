#include "synthesis/abstract.h"
#include "property.h"
#include "predefined.h"

void synthesis_abstract::resize(uint64_t note) {
	dynamic_cast<sound_sequencer *>(producer_.get())->resize(note);
	return;
}

void synthesis_abstract::resample(uint64_t note, std::vector<int16_t> &pcm) {
	dynamic_cast<sound_sequencer *>(producer_.get())->resample(note, pcm);
	return;
}

int8_t synthesis_abstract::synthesize() {
	create();
	synthesis(
	    CONFIG_UINT64("synthesis", "note"),
	    CONFIG_UINT64("synthesis", "period"));

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