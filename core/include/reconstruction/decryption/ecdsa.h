#pragma once
#include "reconstruction/decryption/abstract.h"

namespace ecdsa {
enum class attack : uint8_t {
	trial = 0,
};

struct curve {
	mpz_t p_;
	mpz_t a_;
	mpz_t b_;
	curve(BIGNUM *p, BIGNUM *a, BIGNUM *b);
	~curve();
};

struct point {
	mpz_t x_;
	mpz_t y_;
	curve *curve_;
	bool operator==(const point &rhs) const;
	bool operator!=(const point &rhs) const;
	point operator+(const point &rhs) const;
	point &operator+=(const point &rhs);
	point &operator=(const point &rhs);
	point(const point &rhs);
	point(BIGNUM *x, BIGNUM *y, curve *curve);
	point();
	~point();
};
}; // namespace ecdsa

class decryption_ecdsa : public decryption_abstract {
protected: /* parameter */
	std::vector<uint8_t> private_key_;
	std::vector<uint8_t> public_key_;

public: /* pem */
	std::string pem();

public: /* setter */
	int8_t setkey(const std::vector<uint8_t> &private_key);
	int8_t setkey(const std::string &private_key);

public: /* attack */
	int8_t calckey(const std::vector<uint8_t> &public_key, ecdsa::attack algorithm);
	int8_t calckey(const std::string &public_key, ecdsa::attack algorithm);

protected: /* attack */
	int8_t trial(const ecdsa::point *public_key, const ecdsa::point *generator, char **scalar);

protected: /* abstract */
	virtual int8_t decryption(const std::vector<uint8_t> &public_key, std::vector<uint8_t> &shared_key) override final;
};