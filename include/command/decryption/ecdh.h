#pragma once
#include "command/abstract.h"

namespace ecdh {
enum class attack : uint8_t;
};

class command_ecdh : public command_abstract {
protected: /* parameter */
	std::string private_pem_;
	std::string in_;
	std::string out_;

public: /* abstract */
	virtual void setup(CLI::App *parent) override;
	virtual void run() override;
};

class command_ecdh_private : public command_ecdh {
protected: /* parameter */
	std::string private_pem_;

public: /* abstract */
	virtual void setup(CLI::App *parent) override final;
	virtual void run() override final;
};

class command_ecdh_public : public command_ecdh {
protected: /* parameter */
	std::string public_pem_;
	std::string method_;
	std::unordered_map<std::string, ecdh::attack> map_;

public: /* abstract */
	virtual void setup(CLI::App *parent) override final;
	virtual void run() override final;
};