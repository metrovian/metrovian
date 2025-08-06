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

	if (EVP_PKEY_decrypt_init(ctx) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(pkey);
		spdlog::error("rsa decryption context init failed");
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(pkey);
		spdlog::error("rsa decryption context set failed");
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	size_t len_update = 0;
	if (EVP_PKEY_decrypt(ctx, nullptr, &len_update, cipher.data(), cipher.size()) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(pkey);
		spdlog::error("rsa decryption context update failed");
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
	spdlog::debug("RSA cipher: \"{}\"", base64(cipher));
	spdlog::debug("RSA plain:  \"{}\"", base64(plain));
	spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
	return 0;
}