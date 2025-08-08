#include "decryption_rsa.h"

int8_t decryption_rsa::setkey(const std::vector<uint8_t> &private_key) {
	spdlog::trace("[enter] {}", __PRETTY_FUNCTION__);
	const uint8_t *ptr = private_key.data();
	EVP_PKEY *pkey = d2i_PrivateKey(EVP_PKEY_RSA, nullptr, &ptr, static_cast<int64_t>(private_key.size()));
	if (pkey == nullptr) {
		spdlog::error("rsa decryption setkey failed");
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	EVP_PKEY_free(pkey);
	private_key_ = private_key;
	spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
	return 0;
}

int8_t decryption_rsa::setkey(const std::string &private_key) {
	spdlog::trace("[enter] {}", __PRETTY_FUNCTION__);
	BIO *bio = BIO_new_mem_buf(private_key.data(), static_cast<int32_t>(private_key.size()));
	if (bio == nullptr) {
		spdlog::critical("[invalid implementation] {}", __PRETTY_FUNCTION__);
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	EVP_PKEY *pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
	BIO_free(bio);
	if (pkey == nullptr) {
		spdlog::error("rsa decryption setkey pem readout failed");
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	int32_t len_key = i2d_PrivateKey(pkey, nullptr);
	if (len_key <= 0) {
		EVP_PKEY_free(pkey);
		spdlog::error("rsa decryption setkey der conversion failed");
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	std::vector<uint8_t> der_key(len_key);
	uint8_t *ptr = der_key.data();
	if (i2d_PrivateKey(pkey, &ptr) != len_key) {
		EVP_PKEY_free(pkey);
		spdlog::critical("[invalid implementation] {}", __PRETTY_FUNCTION__);
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	EVP_PKEY_free(pkey);
	private_key_ = std::move(der_key);
	spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
	return 0;
}

int8_t decryption_rsa::calckey(const std::vector<uint8_t> &public_key, rsa::attack algorithm) {
	return 0;
}

int8_t decryption_rsa::calckey(const std::string &public_key, rsa::attack algorithm) {
	spdlog::trace("[enter] {}", __PRETTY_FUNCTION__);
	BIO *bio = BIO_new_mem_buf(public_key.data(), static_cast<int32_t>(public_key.size()));
	if (bio == nullptr) {
		spdlog::critical("[invalid implementation] {}", __PRETTY_FUNCTION__);
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	EVP_PKEY *pkey_public = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
	BIO_free(bio);
	if (pkey_public == nullptr) {
		spdlog::error("rsa decryption calckey pem readout failed");
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	EVP_PKEY_CTX *ctx_pub = EVP_PKEY_CTX_new(pkey_public, nullptr);
	if (ctx_pub == nullptr) {
		EVP_PKEY_free(pkey_public);
		spdlog::critical("[invalid implementation] {}", __PRETTY_FUNCTION__);
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	} else if (EVP_PKEY_get_base_id(pkey_public) != EVP_PKEY_RSA) {
		EVP_PKEY_free(pkey_public);
		spdlog::error("rsa decryption calckey baseid readout failed");
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	BIGNUM *n_rsa = nullptr;
	BIGNUM *e_rsa = nullptr;
	EVP_PKEY_get_bn_param(pkey_public, OSSL_PKEY_PARAM_RSA_N, &n_rsa);
	EVP_PKEY_get_bn_param(pkey_public, OSSL_PKEY_PARAM_RSA_E, &e_rsa);
	if (n_rsa == nullptr) {
		EVP_PKEY_free(pkey_public);
		spdlog::error("rsa decryption calckey modulus readout failed");
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	} else if (e_rsa == nullptr) {
		EVP_PKEY_free(pkey_public);
		spdlog::error("rsa decryption calckey exponent readout failed");
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	BN_CTX *ctx_rsa = BN_CTX_new();
	BIGNUM *p_rsa = BN_new();
	BIGNUM *q_rsa = BN_new();
	switch (algorithm) {
	case rsa::attack::trial: {
		BIGNUM *trial = BN_new();
		BIGNUM *remainder = BN_new();
		while (BN_cmp(trial, n_rsa) < 0) {
			BN_mod(remainder, n_rsa, trial, ctx_rsa);
			if (BN_is_zero(remainder)) {
				BN_copy(p_rsa, trial);
				BN_div(q_rsa, nullptr, n_rsa, p_rsa, ctx_rsa);
				break;
			}
		}

		BN_free(trial);
		BN_free(remainder);
		break;
	}

	default:
		spdlog::critical("[invalid implementation] {}", __PRETTY_FUNCTION__);
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	BIGNUM *phi_rsa = BN_new();
	BIGNUM *p1_rsa = BN_new();
	BIGNUM *q1_rsa = BN_new();
	BIGNUM *d_rsa = BN_new();
	BN_sub(p1_rsa, p_rsa, BN_value_one());
	BN_sub(q1_rsa, q_rsa, BN_value_one());
	BN_mul(phi_rsa, p1_rsa, q1_rsa, ctx_rsa);
	if (BN_mod_inverse(d_rsa, e_rsa, phi_rsa, ctx_rsa) == nullptr) {
		BN_free(phi_rsa);
		BN_free(p1_rsa);
		BN_free(q1_rsa);
		BN_free(p_rsa);
		BN_free(q_rsa);
		BN_free(d_rsa);
		BN_CTX_free(ctx_rsa);
		EVP_PKEY_free(pkey_public);
		spdlog::error("rsa decryption calckey modular inverse failed");
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	OSSL_PARAM_BLD *param_bld = OSSL_PARAM_BLD_new();
	OSSL_PARAM *param = nullptr;
	EVP_PKEY *pkey_private = nullptr;
	EVP_PKEY_CTX *ctx_private = EVP_PKEY_CTX_new_from_name(nullptr, "RSA", nullptr);
	BIGNUM *coefficient = BN_new();
	BIGNUM *exponent1 = BN_new();
	BIGNUM *exponent2 = BN_new();
	BN_mod_inverse(coefficient, q_rsa, p_rsa, ctx_rsa);
	BN_mod(exponent1, d_rsa, p1_rsa, ctx_rsa);
	BN_mod(exponent2, d_rsa, q1_rsa, ctx_rsa);
	OSSL_PARAM_BLD_push_BN(param_bld, OSSL_PKEY_PARAM_RSA_N, n_rsa);
	OSSL_PARAM_BLD_push_BN(param_bld, OSSL_PKEY_PARAM_RSA_E, e_rsa);
	OSSL_PARAM_BLD_push_BN(param_bld, OSSL_PKEY_PARAM_RSA_D, d_rsa);
	OSSL_PARAM_BLD_push_BN(param_bld, "rsa-coefficient1", coefficient);
	OSSL_PARAM_BLD_push_BN(param_bld, "rsa-factor1", p_rsa);
	OSSL_PARAM_BLD_push_BN(param_bld, "rsa-factor2", q_rsa);
	OSSL_PARAM_BLD_push_BN(param_bld, "rsa-exponent1", exponent1);
	OSSL_PARAM_BLD_push_BN(param_bld, "rsa-exponent2", exponent2);
	param = OSSL_PARAM_BLD_to_param(param_bld);
	EVP_PKEY_fromdata_init(ctx_private);
	EVP_PKEY_fromdata(ctx_private, &pkey_private, EVP_PKEY_KEYPAIR, param);
	BIO *mem = BIO_new(BIO_s_mem());
	if (mem == nullptr) {
		EVP_PKEY_free(pkey_public);
		EVP_PKEY_free(pkey_private);
		BN_CTX_free(ctx_rsa);
		spdlog::critical("[invalid implementation] {}", __PRETTY_FUNCTION__);
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	if (i2d_PrivateKey_bio(mem, pkey_private) != 1) {
		BIO_free(mem);
		EVP_PKEY_free(pkey_public);
		EVP_PKEY_free(pkey_private);
		BN_CTX_free(ctx_rsa);
		spdlog::error("rsa decryption calckey der conversion failed");
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	BUF_MEM *buf_mem = nullptr;
	BIO_get_mem_ptr(mem, &buf_mem);
	if (buf_mem == nullptr) {
		BIO_free(mem);
		EVP_PKEY_free(pkey_public);
		EVP_PKEY_free(pkey_private);
		BN_CTX_free(ctx_rsa);
		spdlog::critical("[invalid implementation] {}", __PRETTY_FUNCTION__);
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	} else if (buf_mem->data == nullptr) {
		BIO_free(mem);
		EVP_PKEY_free(pkey_public);
		EVP_PKEY_free(pkey_private);
		BN_CTX_free(ctx_rsa);
		spdlog::error("rsa decryption calckey der allocation failed");
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	private_key_.clear();
	private_key_.assign(buf_mem->data, buf_mem->data + buf_mem->length);
	BIO_free(mem);
	EVP_PKEY_free(pkey_public);
	EVP_PKEY_free(pkey_private);
	EVP_PKEY_CTX_free(ctx_private);
	BN_free(exponent1);
	BN_free(exponent2);
	BN_free(coefficient);
	BN_free(phi_rsa);
	BN_free(p1_rsa);
	BN_free(p_rsa);
	BN_free(q_rsa);
	BN_free(d_rsa);
	BN_free(n_rsa);
	BN_free(e_rsa);
	BN_CTX_free(ctx_rsa);
	OSSL_PARAM_free(param);
	OSSL_PARAM_BLD_free(param_bld);
	spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
	return 0;
}

int8_t decryption_rsa::decryption(const std::vector<uint8_t> &cipher, std::vector<uint8_t> &plain) {
	spdlog::trace("[enter] {}", __PRETTY_FUNCTION__);
	const uint8_t *ptr = private_key_.data();
	EVP_PKEY *pkey = d2i_PrivateKey(EVP_PKEY_RSA, nullptr, &ptr, static_cast<int32_t>(private_key_.size()));
	if (pkey == nullptr) {
		spdlog::error("rsa decryption der conversion failed");
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, nullptr);
	if (ctx == nullptr) {
		EVP_PKEY_free(pkey);
		spdlog::critical("[invalid implementation] {}", __PRETTY_FUNCTION__);
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	int32_t pkey_bits = EVP_PKEY_get_bits(pkey);
	if (EVP_PKEY_decrypt_init(ctx) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(pkey);
		spdlog::error("rsa-{} decryption context init failed", pkey_bits);
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(pkey);
		spdlog::error("rsa-{} decryption context set failed", pkey_bits);
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	size_t len_update = 0;
	if (EVP_PKEY_decrypt(ctx, nullptr, &len_update, cipher.data(), cipher.size()) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(pkey);
		spdlog::error("rsa-{} decryption context update failed", pkey_bits);
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	plain.clear();
	plain.resize(len_update);
	if (EVP_PKEY_decrypt(ctx, plain.data(), &len_update, cipher.data(), cipher.size()) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(pkey);
		spdlog::critical("[invalid implementation] {}", __PRETTY_FUNCTION__);
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	plain.resize(len_update);
	EVP_PKEY_CTX_free(ctx);
	EVP_PKEY_free(pkey);
	spdlog::debug("rsa-{} cipher: \"{}\"", pkey_bits, base64(cipher));
	spdlog::debug("rsa-{} plain:  \"{}\"", pkey_bits, base64(plain));
	spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
	return 0;
}