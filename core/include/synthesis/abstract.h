#pragma once
#include "sound/factory.h"
#include "sound/player.h"
#include "sound/sequencer.h"

class synthesis_abstract : public sound_factory {
protected: /* setter */
	void resize(uint64_t note);
	void resample(uint64_t note, std::vector<int16_t> &pcm);

public: /* overload */
	int8_t synthesize();
	int8_t synthesize(const std::string &path);

public: /* abstract */
	virtual ~synthesis_abstract() {}

protected: /* abstract */
	virtual void synthesis(uint64_t note, uint64_t period) = 0;

protected: /* abstract */
	virtual std::unique_ptr<sound_producer> create_producer() override final;
	virtual std::unique_ptr<sound_consumer> create_consumer() override final;
};