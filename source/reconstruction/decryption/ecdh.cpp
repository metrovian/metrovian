#include "reconstruction/decryption/ecdh.h"
#include "property.h"
#include "predefined.h"

ecdh::curve::curve(BIGNUM *p, BIGNUM *a, BIGNUM *b) {
	char *p_hexstr = BN_bn2hex(p);
	char *a_hexstr = BN_bn2hex(a);
	char *b_hexstr = BN_bn2hex(b);
	mpz_inits(p_, a_, b_, nullptr);
	mpz_set_str(p_, p_hexstr, 16);
	mpz_set_str(a_, a_hexstr, 16);
	mpz_set_str(b_, b_hexstr, 16);
	OPENSSL_free(p_hexstr);
	OPENSSL_free(a_hexstr);
	OPENSSL_free(b_hexstr);
}

ecdh::curve::~curve() {
	mpz_clears(p_, a_, b_, nullptr);
}

bool ecdh::point::operator==(const ecdh::point &rhs) const {
	return !(*this != rhs);
}

bool ecdh::point::operator!=(const ecdh::point &rhs) const {
	if (mpz_cmp(x_, rhs.x_) == 0) {
		if (mpz_cmp(y_, rhs.y_) == 0) {
			if (curve_ == rhs.curve_) {
				return false;
			}
		}
	}

	return true;
}

ecdh::point &ecdh::point::operator+=(const ecdh::point &rhs) {
	if (this->curve_ != rhs.curve_) {
		LOG_ARGUMENT(curve);
		return *this;
	}

	mpz_t x;
	mpz_t y;
	mpz_t lambda;
	mpz_t num;
	mpz_t denom;
	mpz_t inverse;
	mpz_t tmp;
	mpz_inits(x, y, lambda, num, denom, inverse, tmp, nullptr);
	if (mpz_cmp_ui(x_, 0) == 0) {
		if (mpz_cmp_ui(y_, 0) == 0) {
			mpz_clears(x, y, lambda, num, denom, inverse, tmp, nullptr);
			return *this;
		}
	}

	if (mpz_cmp_ui(rhs.x_, 0) == 0) {
		if (mpz_cmp_ui(rhs.y_, 0) == 0) {
			mpz_set(x_, rhs.x_);
			mpz_set(y_, rhs.y_);
			mpz_clears(x, y, lambda, num, denom, inverse, tmp, nullptr);
			return *this;
		}
	}

	if (mpz_cmp(x_, rhs.x_) == 0) {
		mpz_add(tmp, y_, rhs.y_);
		mpz_mod(tmp, tmp, curve_->p_);
		if (mpz_sgn(tmp) == 0) {
			mpz_set_ui(x_, 0);
			mpz_set_ui(y_, 0);
			mpz_clears(x, y, lambda, num, denom, inverse, tmp, nullptr);
			return *this;
		}
	}

	if (*this == rhs) {
		mpz_mul(num, x_, x_);
		mpz_mul_ui(num, num, 3);
		mpz_add(num, num, curve_->a_);
		mpz_mod(num, num, curve_->p_);
		mpz_mul_ui(denom, y_, 2);
		mpz_invert(inverse, denom, curve_->p_);
		mpz_mul(lambda, num, inverse);
		mpz_mod(lambda, lambda, curve_->p_);
	} else {
		mpz_sub(num, rhs.y_, y_);
		mpz_sub(denom, rhs.x_, x_);
		mpz_mod(num, num, curve_->p_);
		mpz_mod(denom, denom, curve_->p_);
		mpz_invert(inverse, denom, curve_->p_);
		mpz_mul(lambda, num, inverse);
		mpz_mod(lambda, lambda, curve_->p_);
	}

	mpz_mul(x, lambda, lambda);
	mpz_sub(x, x, x_);
	mpz_sub(x, x, rhs.x_);
	mpz_mod(x, x, curve_->p_);
	mpz_sub(y, x_, x);
	mpz_mul(y, lambda, y);
	mpz_sub(y, y, y_);
	mpz_mod(y, y, curve_->p_);
	mpz_set(x_, x);
	mpz_set(y_, y);
	mpz_clears(x, y, lambda, num, denom, inverse, tmp, nullptr);
	return *this;
}

ecdh::point ecdh::point::operator+(const ecdh::point &rhs) const {
	return ecdh::point(*this) += rhs;
}

ecdh::point &ecdh::point::operator=(const ecdh::point &rhs) {
	mpz_set(x_, rhs.x_);
	mpz_set(y_, rhs.y_);
	curve_ = rhs.curve_;
	return *this;
}

ecdh::point::point(const ecdh::point &rhs) {
	mpz_inits(x_, y_, nullptr);
	mpz_set(x_, rhs.x_);
	mpz_set(y_, rhs.y_);
	curve_ = rhs.curve_;
}

ecdh::point::point(BIGNUM *x, BIGNUM *y, ecdh::curve *curve) {
	char *x_hexstr = BN_bn2hex(x);
	char *y_hexstr = BN_bn2hex(y);
	mpz_inits(x_, y_, nullptr);
	mpz_set_str(x_, x_hexstr, 16);
	mpz_set_str(y_, y_hexstr, 16);
	curve_ = curve;
	OPENSSL_free(x_hexstr);
	OPENSSL_free(y_hexstr);
}

ecdh::point::point() {
	mpz_inits(x_, y_, nullptr);
	mpz_set_ui(x_, 0);
	mpz_set_ui(y_, 0);
	curve_ = nullptr;
}

ecdh::point::~point() {
	mpz_clears(x_, y_, nullptr);
}

int8_t decryption_ecdh::setkey(const std::vector<uint8_t> &private_key) {
	LOG_ENTER();
	const uint8_t *ptr = private_key.data();
	EVP_PKEY *pkey = d2i_AutoPrivateKey(nullptr, &ptr, static_cast<int64_t>(private_key.size()));
	if (pkey == nullptr) {
		LOG_CONDITION(d2i_AutoPrivateKey == nullptr);
		LOG_EXIT();
		return -1;
	}

	if (EVP_PKEY_base_id(pkey) != EVP_PKEY_EC) {
		EVP_PKEY_free(pkey);
		LOG_CONDITION(EVP_PKEY_base_id != EVP_PKEY_EC);
		LOG_EXIT();
		return -2;
	}

	EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, nullptr);
	if (ctx == nullptr) {
		LOG_CONDITION(EVP_PKEY_CTX_new == nullptr);
		LOG_EXIT();
		return -3;
	}

	if (EVP_PKEY_param_check(ctx) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		LOG_CONDITION(EVP_PKEY_param_check <= 0);
		LOG_EXIT();
		return -4;
	}

	private_key_ = private_key;
	EVP_PKEY_CTX_free(ctx);
	EVP_PKEY_free(pkey);
	LOG_EXIT();
	return 0;
}

int8_t decryption_ecdh::setkey(const std::string &private_key) {
	BIO *bio = BIO_new_mem_buf(private_key.data(), static_cast<int32_t>(private_key.size()));
	if (bio == nullptr) {
		LOG_CONDITION(BIO_new_mem_buf == nullptr);
		LOG_EXIT();
		return -1;
	}

	EVP_PKEY *pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
	BIO_free(bio);
	if (pkey == nullptr) {
		LOG_CONDITION(PEM_read_bio_PrivateKey == nullptr);
		LOG_EXIT();
		return -2;
	}

	int32_t len_key = i2d_PrivateKey(pkey, nullptr);
	if (len_key <= 0) {
		EVP_PKEY_free(pkey);
		LOG_CONDITION(i2d_PrivateKey <= 0);
		LOG_EXIT();
		return -3;
	}

	std::vector<uint8_t> der_key(len_key);
	uint8_t *ptr = der_key.data();
	if (i2d_PrivateKey(pkey, &ptr) != len_key) {
		EVP_PKEY_free(pkey);
		LOG_CONDITION(i2d_PrivateKey <= 0);
		LOG_EXIT();
		return -4;
	}

	private_key_ = std::move(der_key);
	EVP_PKEY_free(pkey);
	LOG_EXIT();
	return 0;
}

int8_t decryption_ecdh::calckey(const std::vector<uint8_t> &public_key, ecdh::attack algorithm) {
	return calckey(base64(public_key), algorithm);
}

int8_t decryption_ecdh::calckey(const std::string &public_key, ecdh::attack algorithm) {
	LOG_ENTER();
	int8_t retcode = 0;
	BIO *bio = nullptr;
	EVP_PKEY_CTX *ctx_public = nullptr;
	EVP_PKEY_CTX *ctx_private = nullptr;
	EVP_PKEY *pkey_public = nullptr;
	EVP_PKEY *pkey_private = nullptr;
	BN_CTX *ctx_ecdh = nullptr;
	BIGNUM *p_ecdh = nullptr;
	BIGNUM *a_ecdh = nullptr;
	BIGNUM *b_ecdh = nullptr;
	BIGNUM *px_ecdh = nullptr;
	BIGNUM *py_ecdh = nullptr;
	BIGNUM *gx_ecdh = nullptr;
	BIGNUM *gy_ecdh = nullptr;
	uint8_t *g_ecdh = nullptr;
	size_t len_ecdh = 0;
	char *dx_hexstr = nullptr;
	char *dy_hexstr = nullptr;
	ecdh::curve *curve = nullptr;
	ecdh::point *point_public = nullptr;
	ecdh::point *point_private = nullptr;
	bio = BIO_new_mem_buf(public_key.data(), static_cast<int32_t>(public_key.size()));
	if (bio == nullptr) {
		LOG_CONDITION(BIO_new_mem_buf == nullptr);
		RETURN_CLEANUP(retcode, -1);
	}

	pkey_public = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
	if (pkey_public == nullptr) {
		LOG_CONDITION(PEM_read_bio_PUBKEY == nullptr);
		RETURN_CLEANUP(retcode, -2);
	}

	ctx_public = EVP_PKEY_CTX_new(pkey_public, nullptr);
	if (ctx_public == nullptr) {
		LOG_CONDITION(EVP_PKEY_CTX_new == nullptr);
		RETURN_CLEANUP(retcode, -3);
	} else if (EVP_PKEY_get_base_id(pkey_public) != EVP_PKEY_EC) {
		LOG_CONDITION(EVP_PKEY_get_base_id != EVP_PKEY_EC);
		RETURN_CLEANUP(retcode, -4);
	}

	EVP_PKEY_get_bn_param(pkey_public, OSSL_PKEY_PARAM_EC_P, &p_ecdh);
	EVP_PKEY_get_bn_param(pkey_public, OSSL_PKEY_PARAM_EC_A, &a_ecdh);
	EVP_PKEY_get_bn_param(pkey_public, OSSL_PKEY_PARAM_EC_B, &b_ecdh);
	EVP_PKEY_get_bn_param(pkey_public, OSSL_PKEY_PARAM_EC_PUB_X, &px_ecdh);
	EVP_PKEY_get_bn_param(pkey_public, OSSL_PKEY_PARAM_EC_PUB_Y, &py_ecdh);
	if (p_ecdh == nullptr) {
		LOG_CONDITION(EVP_PKEY_get_bn_param(OSSL_PKEY_PARAM_EC_P) == nullptr);
		RETURN_CLEANUP(retcode, -5);
	} else if (a_ecdh == nullptr) {
		LOG_CONDITION(EVP_PKEY_get_bn_param(OSSL_PKEY_PARAM_EC_A) == nullptr);
		RETURN_CLEANUP(retcode, -6);
	} else if (b_ecdh == nullptr) {
		LOG_CONDITION(EVP_PKEY_get_bn_param(OSSL_PKEY_PARAM_EC_B) == nullptr);
		RETURN_CLEANUP(retcode, -7);
	} else if (px_ecdh == nullptr) {
		LOG_CONDITION(EVP_PKEY_get_bn_param(OSSL_PKEY_PARAM_EC_PUB_X) == nullptr);
		RETURN_CLEANUP(retcode, -8);
	} else if (py_ecdh == nullptr) {
		LOG_CONDITION(EVP_PKEY_get_bn_param(OSSL_PKEY_PARAM_EC_PUB_Y) == nullptr);
		RETURN_CLEANUP(retcode, -9);
	}

	if (EVP_PKEY_get_octet_string_param(pkey_public, OSSL_PKEY_PARAM_EC_GENERATOR, nullptr, 0, &len_ecdh) == 0) {
		LOG_CONDITION(EVP_PKEY_get_octet_string_param(OSSL_PKEY_PARAM_EC_GENERATOR) == 0);
		RETURN_CLEANUP(retcode, -10);
	}

	g_ecdh = (unsigned char *)OPENSSL_malloc(len_ecdh);
	if (EVP_PKEY_get_octet_string_param(pkey_public, OSSL_PKEY_PARAM_EC_GENERATOR, g_ecdh, len_ecdh, &len_ecdh) == 0) {
		LOG_CONDITION(EVP_PKEY_get_octet_string_param(OSSL_PKEY_PARAM_EC_GENERATOR) == 0);
		RETURN_CLEANUP(retcode, -11);
	}

	gx_ecdh = BN_bin2bn(g_ecdh + 1, (len_ecdh - 1) / 2, nullptr);
	gy_ecdh = BN_bin2bn(g_ecdh + 1 + (len_ecdh - 1) / 2, (len_ecdh - 1) / 2, nullptr);
	curve = new ecdh::curve(p_ecdh, a_ecdh, b_ecdh);
	point_public = new ecdh::point(px_ecdh, py_ecdh, curve);
	point_private = new ecdh::point(gx_ecdh, gy_ecdh, curve);
	// clang-format off
	switch (algorithm) {
	case ecdh::attack::trial: trial(point_public, point_private); break;
	default:
		LOG_ARGUMENT(algorithm);
		RETURN_CLEANUP(retcode, -12);
	}
	// clang-format on

cleanup:
	// clang-format off
	if (bio) bio = nullptr;
	if (ctx_public) EVP_PKEY_CTX_free(ctx_public);
	if (ctx_private) EVP_PKEY_CTX_free(ctx_private);
	if (pkey_public) EVP_PKEY_free(pkey_public);
	if (pkey_private) EVP_PKEY_free(pkey_private);
	if (ctx_ecdh) BN_CTX_free(ctx_ecdh);
	if (p_ecdh) BN_clear_free(p_ecdh);
	if (a_ecdh) BN_clear_free(a_ecdh);
	if (b_ecdh) BN_clear_free(b_ecdh);
	if (px_ecdh) BN_clear_free(px_ecdh);
	if (py_ecdh) BN_clear_free(py_ecdh);
	if (gx_ecdh) BN_clear_free(gx_ecdh);
	if (gy_ecdh) BN_clear_free(gy_ecdh);
	if (g_ecdh) OPENSSL_free(g_ecdh);
	if (dx_hexstr) OPENSSL_free(dx_hexstr);
	if (dy_hexstr) OPENSSL_free(dy_hexstr);
	if (curve) delete curve;
	if (point_public) delete point_public;
	if (point_private) delete point_private;
	// clang-format on
	LOG_EXIT();
	return 0;
}

int8_t decryption_ecdh::trial(const ecdh::point *public_key, ecdh::point *private_key) {
	LOG_ENTER();
	ecdh::point point_private = *private_key;
	ecdh::point point_generator = *private_key;
	mpz_set_ui(private_key->x_, 0);
	mpz_set_ui(private_key->y_, 0);
	uint64_t max = std::stoull(property_singleton::instance().parse({"decryption", "ecdh", "trial-iteration"}));
	for (uint64_t i = 0; i < max; ++i) {
		if (*public_key == point_private) {
			*private_key = point_private;
			break;
		}

		point_private += point_generator;
	}

	LOG_EXIT();
	return 0;
}

int8_t decryption_ecdh::decryption(const std::vector<uint8_t> &cipher, std::vector<uint8_t> &plain) {
	LOG_ENTER();
	const uint8_t *ptr = private_key_.data();
	EVP_PKEY *pkey = d2i_AutoPrivateKey(nullptr, &ptr, static_cast<int64_t>(private_key_.size()));
	if (pkey == nullptr) {
		LOG_CONDITION(d2i_AutoPrivateKey == nullptr);
		LOG_EXIT();
		return -1;
	}

	const uint8_t *ptr_peer = cipher.data();
	EVP_PKEY *pkey_peer = d2i_PUBKEY(nullptr, &ptr_peer, static_cast<int64_t>(cipher.size()));
	if (pkey_peer == nullptr) {
		EVP_PKEY_free(pkey);
		LOG_CONDITION(d2i_PUBKEY == nullptr);
		LOG_EXIT();
		return -2;
	}

	EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, nullptr);
	if (ctx == nullptr) {
		EVP_PKEY_free(pkey);
		EVP_PKEY_free(pkey_peer);
		LOG_CONDITION(EVP_PKEY_CTX_new == nullptr);
		LOG_EXIT();
		return -3;
	}

	if (EVP_PKEY_derive_init(ctx) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(pkey);
		EVP_PKEY_free(pkey_peer);
		LOG_CONDITION(EVP_PKEY_derive_init <= 0);
		LOG_EXIT();
		return -4;
	}

	if (EVP_PKEY_derive_set_peer(ctx, pkey_peer) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(pkey);
		EVP_PKEY_free(pkey_peer);
		LOG_CONDITION(EVP_PKEY_derive_set_peer <= 0);
		LOG_EXIT();
		return -5;
	}

	size_t len_derive = 0;
	if (EVP_PKEY_derive(ctx, nullptr, &len_derive) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(pkey);
		EVP_PKEY_free(pkey_peer);
		LOG_CONDITION(EVP_PKEY_derive <= 0);
		LOG_EXIT();
		return -6;
	}

	plain.resize(len_derive);
	if (EVP_PKEY_derive(ctx, plain.data(), &len_derive) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(pkey);
		EVP_PKEY_free(pkey_peer);
		LOG_CONDITION(EVP_PKEY_derive <= 0);
		LOG_EXIT();
		return -7;
	}

	plain.resize(len_derive);
	EVP_PKEY_CTX_free(ctx);
	EVP_PKEY_free(pkey);
	EVP_PKEY_free(pkey_peer);
	spdlog::debug("ecdh cipher: \"{}\"", base64(cipher));
	spdlog::debug("ecdh plain: \"{}\"", base64(plain));
	LOG_EXIT();
	return 0;
}
