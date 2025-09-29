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

	if (len_derive > 0) {
		plain.clear();
		plain.resize(len_derive);
		if (EVP_PKEY_derive(ctx, plain.data(), &len_derive) <= 0) {
			EVP_PKEY_CTX_free(ctx);
			EVP_PKEY_free(pkey);
			EVP_PKEY_free(pkey_peer);
			LOG_CONDITION(EVP_PKEY_derive <= 0);
			LOG_EXIT();
			return -7;
		}

		if (len_derive > 0) {
			plain.resize(len_derive);
		}
	}

	EVP_PKEY_CTX_free(ctx);
	EVP_PKEY_free(pkey);
	EVP_PKEY_free(pkey_peer);
	spdlog::debug("ecdh cipher: \"{}\"", base64(cipher));
	spdlog::debug("ecdh plain: \"{}\"", base64(plain));
	LOG_EXIT();
	return 0;
}
