#pragma once
#include "command/abstract.h"

class command_avcodec : public command_abstract {
protected: /* parameter */
	std::string in_;

public: /* abstract */
	virtual void setup(CLI::App *parent) override final;
	virtual void run() override final;
};