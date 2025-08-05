#include "decryption_aes.h"

int8_t decryption_aes::setkey(const std::vector<uint8_t> &key) {
	spdlog::trace("[enter] {}", __PRETTY_FUNCTION__);
	if (key.size() != key_size()) {
		spdlog::error("aes{} decryption setkey failed: {}", static_cast<size_t>(8) * key_size(), key.size());
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	key_ = key;
	spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
	return 0;
}

int8_t decryption_aes::setiv(const std::vector<uint8_t> &iv) {
	spdlog::trace("[enter] {}", __PRETTY_FUNCTION__);
	if (iv.size() != iv_size()) {
		spdlog::error("aes{} decryption setiv failed: {}", static_cast<size_t>(8) * key_size(), iv.size());
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	iv_ = iv;
	spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
	return 0;
}

int8_t decryption_aes::setkey(const std::string &key) {
	spdlog::trace("[enter] {}", __PRETTY_FUNCTION__);
	std::vector<uint8_t> decoded = base64(key);
	if (decoded.size() != key_size()) {
		spdlog::error("aes{} decryption setkey failed: {}", static_cast<size_t>(8) * key_size(), decoded.size());
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	key_ = decoded;
	spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
	return 0;
}

int8_t decryption_aes::setiv(const std::string &iv) {
	spdlog::trace("[enter] {}", __PRETTY_FUNCTION__);
	std::vector<uint8_t> decoded = base64(iv);
	if (decoded.size() != iv_size()) {
		spdlog::error("aes{} decryption setiv failed: {}", static_cast<size_t>(8) * key_size(), decoded.size());
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	iv_ = decoded;
	spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
	return 0;
}

int8_t decryption_aes256_cbc::decryption(const std::vector<uint8_t> &cipher, std::vector<uint8_t> &plain) {
	spdlog::trace("[enter] {}", __PRETTY_FUNCTION__);
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	if (ctx == nullptr) {
		spdlog::critical("[invalid implementation] {}", __PRETTY_FUNCTION__);
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	plain.clear();
	plain.resize(cipher.size() + EVP_MAX_BLOCK_LENGTH);
	if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key_.data(), iv_.data()) != 1) {
		EVP_CIPHER_CTX_free(ctx);
		spdlog::error("aes256-cbc decryption context init failed");
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	int32_t len_update = 0;
	int32_t len_final = 0;
	if (EVP_DecryptUpdate(ctx, plain.data(), &len_update, cipher.data(), static_cast<int32_t>(cipher.size())) != 1) {
		EVP_CIPHER_CTX_free(ctx);
		spdlog::error("aes256-cbc decryption context update failed");
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	if (EVP_DecryptFinal_ex(ctx, plain.data() + len_update, &len_final) != 1) {
		EVP_CIPHER_CTX_free(ctx);
		spdlog::error("aes256-cbc decryption context final failed");
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	plain.resize(static_cast<size_t>(len_update) + static_cast<size_t>(len_final));
	EVP_CIPHER_CTX_free(ctx);
	spdlog::debug("aes256-cbc cipher: \"{}\"", base64(cipher));
	spdlog::debug("aes256-cbc key:    \"{}\"", base64(key_));
	spdlog::debug("aes256-cbc iv:     \"{}\"", base64(iv_));
	spdlog::debug("aes256-cbc plain:  \"{}\"", base64(plain));
	spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
	return 0;
}

int8_t decryption_aes256_ctr::decryption(const std::vector<uint8_t> &cipher, std::vector<uint8_t> &plain) {
	spdlog::trace("[enter] {}", __PRETTY_FUNCTION__);
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	if (ctx == nullptr) {
		spdlog::critical("[invalid implementation] {}", __PRETTY_FUNCTION__);
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	plain.clear();
	plain.resize(cipher.size());
	if (EVP_DecryptInit_ex(ctx, EVP_aes_256_ctr(), nullptr, key_.data(), iv_.data()) != 1) {
		EVP_CIPHER_CTX_free(ctx);
		spdlog::error("aes256-ctr decryption context init failed");
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	int32_t len_update = 0;
	if (EVP_DecryptUpdate(ctx, plain.data(), &len_update, cipher.data(), static_cast<int32_t>(cipher.size())) != 1) {
		EVP_CIPHER_CTX_free(ctx);
		spdlog::error("aes256-ctr decryption context update failed");
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	plain.resize(static_cast<size_t>(len_update));
	EVP_CIPHER_CTX_free(ctx);
	spdlog::debug("aes256-ctr cipher: \"{}\"", base64(cipher));
	spdlog::debug("aes256-ctr key:    \"{}\"", base64(key_));
	spdlog::debug("aes256-ctr iv:     \"{}\"", base64(iv_));
	spdlog::debug("aes256-ctr plain:  \"{}\"", base64(plain));
	spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
	return 0;
}