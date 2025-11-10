#pragma once
#include "command/abstract.h"

namespace ecdsa {
enum class attack : uint8_t;
};

class command_ecdsa : public command_abstract {
protected: /* parameter */
	std::string in_;
	std::string out_;

public: /* abstract */
	virtual void setup(CLI::App *parent) override;
	virtual void run() override;
};

class command_ecdsa_private : public command_ecdsa {
protected: /* parameter */
	std::string private_pem_;
	std::string function_;
	std::unordered_map<
	    std::string,
	    std::function<std::vector<uint8_t>(const std::vector<uint8_t> &)>>
	    map_;

public: /* abstract */
	virtual void setup(CLI::App *parent) override final;
	virtual void run() override final;
};

class command_ecdsa_public : public command_ecdsa {
protected: /* parameter */
	std::string method_;
	std::unordered_map<std::string, ecdsa::attack> map_;

public: /* abstract */
	virtual void setup(CLI::App *parent) override final;
	virtual void run() override final;
};