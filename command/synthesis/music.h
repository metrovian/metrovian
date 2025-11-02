#pragma once
#include "abstract.h"

namespace music {
enum class waveform : uint8_t {
	sin = 0,
	saw = 1,
	square = 2,
};
}; // namespace music

class command_music : public command_abstract {
protected: /* parameter */
	std::string model_;
	std::unordered_map<std::string, music::waveform> map_;

public: /* abstract */
	virtual void setup(CLI::App *parent) override final;
	virtual void run() override final;
};