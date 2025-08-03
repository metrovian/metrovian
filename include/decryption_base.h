#pragma once
#include "decryption_abstract.h"

class decryption_base64 : public decryption_abstract {
public: /* abstract */
	virtual int8_t decrypt(const std::vector<uint8_t> &cipher, std::vector<uint8_t> &plain) override final;
};