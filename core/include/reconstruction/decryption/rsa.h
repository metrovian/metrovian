#pragma once
#include "reconstruction/decryption/abstract.h"

namespace rsa {
enum class attack : uint8_t {
	trial = 0,
	fermat = 1,
	pollards_rho = 2,
	pollards_p1 = 3,
	williams_p1 = 4,
};
}; // namespace rsa

class decryption_rsa : public decryption_abstract {
protected: /* parameter */
	std::vector<uint8_t> private_key_;
	std::vector<uint8_t> public_key_;

public: /* pem */
	std::string pem();

public: /* setter */
	int8_t setkey(const std::vector<uint8_t> &private_key);
	int8_t setkey(const std::string &private_key);

public: /* attack */
	int8_t calckey(const std::vector<uint8_t> &public_key, rsa::attack algorithm);
	int8_t calckey(const std::string &public_key, rsa::attack algorithm);

protected: /* attack */
	int8_t trial(const char *modulus, char **prime1, char **prime2);
	int8_t fermat(const char *modulus, char **prime1, char **prime2);
	int8_t pollards_rho(const char *modulus, char **prime1, char **prime2);
	int8_t pollards_p1(const char *modulus, char **prime1, char **prime2);
	int8_t williams_p1(const char *modulus, char **prime1, char **prime2);

protected: /* abstract */
	virtual int8_t decryption(const std::vector<uint8_t> &cipher, std::vector<uint8_t> &plain) override final;
};