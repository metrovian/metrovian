#include "reconstruction/decryption/ecdh.h"
#include "predefined.h"

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
	char *px_hexstr = nullptr;
	char *py_hexstr = nullptr;
	char *gx_hexstr = nullptr;
	char *gy_hexstr = nullptr;
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

	if (EVP_PKEY_get_octet_string_param(pkey_public, OSSL_PKEY_PARAM_EC_GENERATOR, nullptr, 0, &len_ecdh) != 0) {
		LOG_CONDITION(EVP_PKEY_get_octet_string_param(OSSL_PKEY_PARAM_EC_GENERATOR) != 0);
		RETURN_CLEANUP(retcode, -10);
	}

	g_ecdh = (unsigned char *)OPENSSL_malloc(len_ecdh);
	if (EVP_PKEY_get_octet_string_param(pkey_public, OSSL_PKEY_PARAM_EC_GENERATOR, g_ecdh, len_ecdh, &len_ecdh) != 0) {
		LOG_CONDITION(EVP_PKEY_get_octet_string_param(OSSL_PKEY_PARAM_EC_GENERATOR) != 0);
		RETURN_CLEANUP(retcode, -11);
	}

	gx_ecdh = BN_bin2bn(g_ecdh + 1, (len_ecdh - 1) / 2, nullptr);
	gy_ecdh = BN_bin2bn(g_ecdh + 1 + (len_ecdh - 1) / 2, (len_ecdh - 1) / 2, nullptr);

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
	if (px_hexstr) OPENSSL_free(px_hexstr);
	if (py_hexstr) OPENSSL_free(py_hexstr);
	if (gx_hexstr) OPENSSL_free(gx_hexstr);
	if (gy_hexstr) OPENSSL_free(gy_hexstr);
	// clang-format on
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
