#pragma once
#include "abstract.h"

namespace music {
enum class waveform : uint8_t {
	sin = 0,
	saw = 1,
	square = 2,
	unison = 3,
	hammond = 4,
};
}; // namespace music

class command_music : public command_abstract {
protected: /* parameter */
	std::string waveform_;
	std::unordered_map<std::string, music::waveform> map_;
	std::vector<double> params_;

public: /* abstract */
	virtual void setup(CLI::App *parent) override final;
	virtual void run() override final;
};