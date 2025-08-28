#pragma once
#include "command/abstract.h"

class command_user : public command_abstract {
protected: /* parameter */
	std::string function_;
	std::string in_;
	size_t iter_;
	double eps_;

public: /* abstract */
	virtual void setup(CLI::App *parent) override final;
	virtual void run() override final;
};