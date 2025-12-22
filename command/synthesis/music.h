#pragma once
#include "command/abstract.h"

namespace music {
enum class method : uint8_t {
	add = 0,
	fm = 1,
};
}; // namespace music

class command_music : public command_abstract {
protected: /* parameter */
	std::string method_;
	std::string in_;
	std::unordered_map<std::string, music::method> map_;

public: /* abstract */
	virtual void setup(CLI::App *parent) override final;
	virtual void run() override final;
};