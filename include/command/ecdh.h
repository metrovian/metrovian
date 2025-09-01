#pragma once
#include "command/abstract.h"

class command_ecdh : public command_abstract {
protected: /* parameter */
	std::string private_pem_;
	std::string in_;
	std::string out_;

public: /* abstract */
	virtual void setup(CLI::App *parent) override;
	virtual void run() override;
};
