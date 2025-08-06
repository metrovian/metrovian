#pragma once
#include "decryption_abstract.h"

class decryption_rsa : public decryption_abstract {
protected: /* parameter */
	std::vector<uint8_t> private_key_;
	std::vector<uint8_t> public_key_;

public: /* setter */
	int8_t setkey(const std::vector<uint8_t> &private_key);
	int8_t setkey(const std::string &private_key);

protected: /* abstract */
	virtual int8_t decryption(const std::vector<uint8_t> &cipher, std::vector<uint8_t> &plain) override final;
};