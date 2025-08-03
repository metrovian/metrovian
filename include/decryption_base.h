#pragma once
#include "decryption_abstract.h"

class decryption_base64 : public decryption_abstract {
protected: /* abstract */
	virtual int8_t decryption(const std::vector<uint8_t> &cipher, std::vector<uint8_t> &plain) override final;
};