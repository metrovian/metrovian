#pragma once
#include "reconstruction/decryption/abstract.h"

namespace ecdh {
enum class attack : uint8_t {
	trial = 0,
};
};

class decryption_ecdh : public decryption_abstract {
protected: /* parameter */
	std::vector<uint8_t> private_key_;
	std::vector<uint8_t> public_key_;

public: /* setter */
	int8_t setkey(const std::vector<uint8_t> &private_key);
	int8_t setkey(const std::string &private_key);

public: /* attack */
	int8_t calckey(const std::vector<uint8_t> &public_key, ecdh::attack algorithm);
	int8_t calckey(const std::string &public_key, ecdh::attack algorithm);

protected: /* abstract */
	virtual int8_t decryption(const std::vector<uint8_t> &public_key, std::vector<uint8_t> &shared_key) override final;
};