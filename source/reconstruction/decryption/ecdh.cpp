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

std::string decryption_ecdh::pem() {
	LOG_ENTER();
	const uint8_t *ptr = private_key_.data();
	EVP_PKEY *pkey = d2i_PrivateKey(EVP_PKEY_EC, nullptr, &ptr, private_key_.size());
	if (pkey == nullptr) {
		LOG_CONDITION(d2i_PrivateKey(EVP_PKEY_EC) == nullptr);
		LOG_EXIT();
		return std::string();
	}

	BIO *bio = BIO_new(BIO_s_mem());
	if (bio == nullptr) {
		EVP_PKEY_free(pkey);
		LOG_CONDITION(BIO_new == nullptr);
		LOG_EXIT();
		return std::string();
	}

	if (PEM_write_bio_PrivateKey(bio, pkey, nullptr, nullptr, 0, nullptr, nullptr) == 0) {
		BIO_free(bio);
		EVP_PKEY_free(pkey);
		LOG_CONDITION(PEM_write_bio_PrivateKey == 0);
		LOG_EXIT();
		return std::string();
	}

	char *ptr_key = nullptr;
	int64_t len_key = BIO_get_mem_data(bio, &ptr_key);
	if (len_key <= 0) {
		BIO_free(bio);
		EVP_PKEY_free(pkey);
		LOG_CONDITION(BIO_get_mem_data <= 0);
		LOG_EXIT();
		return std::string();
	}

	std::string pem_key(ptr_key, len_key);
	BIO_free(bio);
	EVP_PKEY_free(pkey);
	LOG_EXIT();
	return pem_key;
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
	BIGNUM *d_ecdh = nullptr;
	BIGNUM *p_ecdh = nullptr;
	BIGNUM *a_ecdh = nullptr;
	BIGNUM *b_ecdh = nullptr;
	BIGNUM *px_ecdh = nullptr;
	BIGNUM *py_ecdh = nullptr;
	BIGNUM *gx_ecdh = nullptr;
	BIGNUM *gy_ecdh = nullptr;
	uint8_t *g_ecdh = nullptr;
	uint8_t *pub_oct = nullptr;
	size_t len_ecdh = 0;
	size_t len_oct = 0;
	size_t len_name = 64;
	OSSL_PARAM_BLD *param_bld = nullptr;
	OSSL_PARAM *param = nullptr;
	BIO *mem = nullptr;
	BUF_MEM *buf_mem = nullptr;
	char *d_hexstr = nullptr;
	char *curve_name = nullptr;
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
	case ecdh::attack::trial: trial(point_public, point_private, &d_hexstr); break;
	default:
		LOG_ARGUMENT(algorithm);
		RETURN_CLEANUP(retcode, -12);
	}
	// clang-format on
	d_ecdh = BN_new();
	BN_hex2bn(&d_ecdh, d_hexstr);
	if (BN_is_zero(d_ecdh) == 1) {
		LOG_CONDITION(BN_is_zero == 1);
		RETURN_CLEANUP(retcode, -13);
	}

	len_oct = (BN_num_bits(px_ecdh) + 7) / 8;
	pub_oct = (unsigned char *)OPENSSL_malloc(len_oct * 2 + 1);
	pub_oct[0] = 0x04;
	BN_bn2binpad(px_ecdh, pub_oct + 1, len_oct);
	BN_bn2binpad(py_ecdh, pub_oct + 1 + len_oct, len_oct);
	curve_name = (char *)OPENSSL_malloc(len_name);
	if (EVP_PKEY_get_utf8_string_param(pkey_public, OSSL_PKEY_PARAM_GROUP_NAME, curve_name, len_name, &len_name) == 0) {
		LOG_CONDITION(EVP_PKEY_get_utf8_string_param(OSSL_PKEY_PARAM_GROUP_NAME) == 0);
		RETURN_CLEANUP(retcode, -14);
	}

	param_bld = OSSL_PARAM_BLD_new();
	ctx_private = EVP_PKEY_CTX_new_from_name(nullptr, "EC", nullptr);
	OSSL_PARAM_BLD_push_BN(param_bld, OSSL_PKEY_PARAM_PRIV_KEY, d_ecdh);
	OSSL_PARAM_BLD_push_octet_string(param_bld, OSSL_PKEY_PARAM_PUB_KEY, pub_oct, len_oct * 2 + 1);
	OSSL_PARAM_BLD_push_utf8_string(param_bld, OSSL_PKEY_PARAM_GROUP_NAME, curve_name, len_name);
	mem = BIO_new(BIO_s_mem());
	if (mem == nullptr) {
		LOG_CONDITION(BIO_new == nullptr);
		RETURN_CLEANUP(retcode, -15);
	}

	param = OSSL_PARAM_BLD_to_param(param_bld);
	EVP_PKEY_fromdata_init(ctx_private);
	EVP_PKEY_fromdata(ctx_private, &pkey_private, EVP_PKEY_KEYPAIR, param);
	if (i2d_PrivateKey_bio(mem, pkey_private) != 1) {
		LOG_CONDITION(i2d_PrivateKey_bio(EVP_PKEY_KEYPAIR) != 1);
		RETURN_CLEANUP(retcode, -16);
	}

	BIO_get_mem_ptr(mem, &buf_mem);
	if (buf_mem == nullptr) {
		LOG_CONDITION(BIO_get_mem_ptr == nullptr);
		RETURN_CLEANUP(retcode, -17);
	} else if (buf_mem->data == nullptr) {
		LOG_CONDITION(BIO_get_mem_ptr == nullptr);
		RETURN_CLEANUP(retcode, -18);
	}

	private_key_.clear();
	private_key_.assign(buf_mem->data, buf_mem->data + buf_mem->length);

cleanup:
	// clang-format off
	if (bio) BIO_free(bio);
	if (ctx_public) EVP_PKEY_CTX_free(ctx_public);
	if (ctx_private) EVP_PKEY_CTX_free(ctx_private);
	if (pkey_public) EVP_PKEY_free(pkey_public);
	if (pkey_private) EVP_PKEY_free(pkey_private);
	if (d_ecdh) BN_clear_free(d_ecdh);
	if (p_ecdh) BN_clear_free(p_ecdh);
	if (a_ecdh) BN_clear_free(a_ecdh);
	if (b_ecdh) BN_clear_free(b_ecdh);
	if (px_ecdh) BN_clear_free(px_ecdh);
	if (py_ecdh) BN_clear_free(py_ecdh);
	if (gx_ecdh) BN_clear_free(gx_ecdh);
	if (gy_ecdh) BN_clear_free(gy_ecdh);
	if (g_ecdh) OPENSSL_free(g_ecdh);
	if (pub_oct) OPENSSL_free(pub_oct);
	if (param_bld) OSSL_PARAM_BLD_free(param_bld);
	if (param) OSSL_PARAM_free(param);
	if (mem) BIO_free(mem);
	if (d_hexstr) OPENSSL_free(d_hexstr);
	if (curve) delete curve;
	if (point_public) delete point_public;
	if (point_private) delete point_private;
	// clang-format on
	LOG_EXIT();
	return retcode;
}

int8_t decryption_ecdh::trial(const ecdh::point *public_key, const ecdh::point *generator, char **scalar) {
	LOG_ENTER();
	mpz_t d;
	mpz_init(d);
	mpz_set_ui(d, 0);
	ecdh::point point_private = *generator;
	ecdh::point point_generator = *generator;
	uint64_t max = std::stoull(property_singleton::instance().parse({"decryption", "ecdh", "trial-iteration"}));
	for (uint64_t i = 0; i < max; ++i) {
		if (point_private == *public_key) {
			mpz_set_ui(d, i + 1);
			break;
		}

		point_private += point_generator;
	}

	*scalar = mpz_get_str(nullptr, 16, d);
	mpz_clear(d);
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
