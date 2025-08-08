#pragma once
#include "decryption_abstract.h"

namespace rsa {
enum class attack {
	trial = 0,
	fermat = 1,
};
};

class decryption_rsa : public decryption_abstract {
protected: /* parameter */
	std::vector<uint8_t> private_key_;
	std::vector<uint8_t> public_key_;

public: /* setter */
	int8_t setkey(const std::vector<uint8_t> &private_key);
	int8_t setkey(const std::string &private_key);

public: /* attack */
	int8_t calckey(const std::vector<uint8_t> &public_key, rsa::attack algorithm);
	int8_t calckey(const std::string &public_key, rsa::attack algorithm);

protected: /* abstract */
	virtual int8_t decryption(const std::vector<uint8_t> &cipher, std::vector<uint8_t> &plain) override final;
};