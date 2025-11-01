#pragma once
#include "abstract.h"

class command_audio : public command_abstract {
protected: /* parameter */
	std::string in_;
	std::string out_;

public: /* abstract */
	virtual void setup(CLI::App *parent) override final;
	virtual void run() override final;
};