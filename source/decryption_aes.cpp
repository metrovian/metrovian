#include "decryption_aes.h"
#include "decryption_base.h"

int8_t decryption_aes256::setkey(const std::vector<uint8_t> &key) {
	spdlog::trace("[enter] {}", __PRETTY_FUNCTION__);
	if (key.size() != 32) {
		spdlog::error("aes256 decryption setkey failed: {}", key.size());
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	key_ = key;
	spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
	return 0;
}

int8_t decryption_aes256::setiv(const std::vector<uint8_t> &iv) {
	spdlog::trace("[enter] {}", __PRETTY_FUNCTION__);
	if (iv.size() != 16) {
		spdlog::error("aes256 decryption setkey failed: {}", iv.size());
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	iv_ = iv;
	spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
	return 0;
}

int8_t decryption_aes256::setkey(const std::string &key) {
	spdlog::trace("[enter] {}", __PRETTY_FUNCTION__);
	std::vector<uint8_t> decoded;
	if (decryption_base64().decrypt(key, decoded) < 0) {
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	} else if (decoded.size() != 32) {
		spdlog::error("aes256 decryption setkey failed: {}", decoded.size());
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	key_ = decoded;
	spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
	return 0;
}

int8_t decryption_aes256::setiv(const std::string &iv) {
	spdlog::trace("[enter] {}", __PRETTY_FUNCTION__);
	std::vector<uint8_t> decoded;
	if (decryption_base64().decrypt(iv, decoded) < 0) {
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	} else if (decoded.size() != 16) {
		spdlog::error("aes256 decryption setiv failed: {}", decoded.size());
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	iv_ = decoded;
	spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
	return 0;
}

int8_t decryption_aes256_cbc::decryption(const std::vector<uint8_t> &cipher, std::vector<uint8_t> &plain) {
	spdlog::trace("[enter] {}", __PRETTY_FUNCTION__);
	std::vector<uint8_t> decoded;
	if (decryption_base64().decrypt(cipher, decoded) < 0) {
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	if (ctx == nullptr) {
		spdlog::critical("[invalid implementation] {}", __PRETTY_FUNCTION__);
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	plain.resize(decoded.size() + EVP_MAX_BLOCK_LENGTH);
	if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key_.data(), iv_.data()) != 1) {
		EVP_CIPHER_CTX_free(ctx);
		spdlog::error("aes256-cbc decryption context init failed");
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	int32_t len1 = 0;
	int32_t len2 = 0;
	if (EVP_DecryptUpdate(ctx, plain.data(), &len1, decoded.data(), static_cast<int32_t>(decoded.size())) != 1) {
		EVP_CIPHER_CTX_free(ctx);
		spdlog::error("aes256-cbc decryption context update failed");
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	if (EVP_DecryptFinal_ex(ctx, plain.data() + len1, &len2) != 1) {
		EVP_CIPHER_CTX_free(ctx);
		spdlog::error("aes256-cbc decryption context final failed");
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	EVP_CIPHER_CTX_free(ctx);
	plain.resize(len1 + len2);
	spdlog::debug("aes256-cbc cipher: \"{}\"", btoh(decoded));
	spdlog::debug("aes256-cbc key:    \"{}\"", btoh(key_));
	spdlog::debug("aes256-cbc iv:     \"{}\"", btoh(iv_));
	spdlog::debug("aes256-cbc plain:  \"{}\"", btoh(plain));
	spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
	return 0;
}