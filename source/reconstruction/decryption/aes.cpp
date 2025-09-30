#include "reconstruction/decryption/aes.h"
#include "predefined.h"

int8_t decryption_aes::setkey(const std::vector<uint8_t> &key) {
	LOG_ENTER();
	if (key.size() != sizekey()) {
		LOG_ARGUMENT(key);
		LOG_EXIT();
		return -1;
	}

	key_ = key;
	LOG_EXIT();
	return 0;
}

int8_t decryption_aes::setiv(const std::vector<uint8_t> &iv) {
	LOG_ENTER();
	if (iv.size() != sizeiv()) {
		LOG_ARGUMENT(iv);
		LOG_EXIT();
		return -1;
	}

	iv_ = iv;
	LOG_EXIT();
	return 0;
}

int8_t decryption_aes::setkey(const std::string &key) {
	LOG_ENTER();
	std::vector<uint8_t> decoded = base64(key);
	if (decoded.size() != sizekey()) {
		LOG_ARGUMENT(key);
		LOG_EXIT();
		return -1;
	}

	key_ = decoded;
	LOG_EXIT();
	return 0;
}

int8_t decryption_aes::setiv(const std::string &iv) {
	LOG_ENTER();
	std::vector<uint8_t> decoded = base64(iv);
	if (decoded.size() != sizeiv()) {
		LOG_ARGUMENT(iv);
		LOG_EXIT();
		return -1;
	}

	iv_ = decoded;
	LOG_EXIT();
	return 0;
}

int8_t decryption_aes256_cbc::decryption(const std::vector<uint8_t> &cipher, std::vector<uint8_t> &plain) {
	LOG_ENTER();
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	if (ctx == nullptr) {
		LOG_CONDITION(EVP_CIPHER_CTX_new == nullptr);
		LOG_EXIT();
		return -1;
	}

	plain.resize(cipher.size() + EVP_MAX_BLOCK_LENGTH);
	if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key_.data(), iv_.data()) != 1) {
		EVP_CIPHER_CTX_free(ctx);
		LOG_CONDITION(EVP_DecryptInit_ex != 1);
		LOG_EXIT();
		return -2;
	}

	int32_t len_update = 0;
	int32_t len_final = 0;
	if (EVP_DecryptUpdate(ctx, plain.data(), &len_update, cipher.data(), static_cast<int32_t>(cipher.size())) != 1) {
		EVP_CIPHER_CTX_free(ctx);
		LOG_CONDITION(EVP_DecryptUpdate != 1);
		LOG_EXIT();
		return -3;
	}

	if (EVP_DecryptFinal_ex(ctx, plain.data() + len_update, &len_final) != 1) {
		EVP_CIPHER_CTX_free(ctx);
		LOG_CONDITION(EVP_DecryptFinal_ex != 1);
		LOG_EXIT();
		return -4;
	}

	plain.resize(static_cast<size_t>(len_update) + static_cast<size_t>(len_final));
	EVP_CIPHER_CTX_free(ctx);
	spdlog::debug("aes-256-cbc cipher: \"{}\"", base64(cipher));
	spdlog::debug("aes-256-cbc key:    \"{}\"", base64(key_));
	spdlog::debug("aes-256-cbc iv:     \"{}\"", base64(iv_));
	spdlog::debug("aes-256-cbc plain:  \"{}\"", base64(plain));
	LOG_EXIT();
	return 0;
}

int8_t decryption_aes256_ctr::decryption(const std::vector<uint8_t> &cipher, std::vector<uint8_t> &plain) {
	LOG_ENTER();
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	if (ctx == nullptr) {
		LOG_CONDITION(EVP_CIPHER_CTX_new == nullptr);
		LOG_EXIT();
		return -1;
	}

	plain.resize(cipher.size());
	if (EVP_DecryptInit_ex(ctx, EVP_aes_256_ctr(), nullptr, key_.data(), iv_.data()) != 1) {
		EVP_CIPHER_CTX_free(ctx);
		LOG_CONDITION(EVP_DecryptInit_ex != 1);
		LOG_EXIT();
		return -2;
	}

	int32_t len_update = 0;
	if (EVP_DecryptUpdate(ctx, plain.data(), &len_update, cipher.data(), static_cast<int32_t>(cipher.size())) != 1) {
		EVP_CIPHER_CTX_free(ctx);
		LOG_CONDITION(EVP_DecryptUpdate != 1);
		LOG_EXIT();
		return -3;
	}

	plain.resize(static_cast<size_t>(len_update));
	EVP_CIPHER_CTX_free(ctx);
	spdlog::debug("aes-256-ctr cipher: \"{}\"", base64(cipher));
	spdlog::debug("aes-256-ctr key:    \"{}\"", base64(key_));
	spdlog::debug("aes-256-ctr iv:     \"{}\"", base64(iv_));
	spdlog::debug("aes-256-ctr plain:  \"{}\"", base64(plain));
	LOG_EXIT();
	return 0;
}

int8_t decryption_aes256_gcm::decryption(const std::vector<uint8_t> &cipher, std::vector<uint8_t> &plain) {
	LOG_ENTER();
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	if (ctx == nullptr) {
		LOG_CONDITION(EVP_CIPHER_CTX_new == nullptr);
		LOG_EXIT();
		return -1;
	}

	size_t len_tag = 16;
	size_t len_cipher = cipher.size() - len_tag;
	if (len_tag > cipher.size()) {
		len_tag = cipher.size();
		len_cipher = 0;
	}

	const uint8_t *data_tag = cipher.data() + len_cipher;
	const uint8_t *data_cipher = cipher.data();
	plain.resize(len_cipher);
	if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1) {
		EVP_CIPHER_CTX_free(ctx);
		LOG_CONDITION(EVP_DecryptInit_ex != 1);
		LOG_EXIT();
		return -2;
	}

	if (EVP_DecryptInit_ex(ctx, nullptr, nullptr, key_.data(), iv_.data()) != 1) {
		EVP_CIPHER_CTX_free(ctx);
		LOG_CONDITION(EVP_DecryptInit_ex != 1);
		LOG_EXIT();
		return -3;
	}

	int32_t len_update = 0;
	int32_t len_final = 0;
	if (EVP_DecryptUpdate(ctx, plain.data(), &len_update, data_cipher, static_cast<int32_t>(len_cipher)) != 1) {
		EVP_CIPHER_CTX_free(ctx);
		LOG_CONDITION(EVP_DecryptUpdate != 1);
		LOG_EXIT();
		return -4;
	}

	if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, static_cast<int32_t>(len_tag), const_cast<uint8_t *>(data_tag)) != 1) {
		EVP_CIPHER_CTX_free(ctx);
		LOG_CONDITION(EVP_CIPHER_CTX_ctrl != 1);
		LOG_EXIT();
		return -5;
	}

	if (EVP_DecryptFinal_ex(ctx, plain.data() + len_update, &len_final) != 1) {
		EVP_CIPHER_CTX_free(ctx);
		LOG_CONDITION(EVP_DecryptFinal_ex != 1);
		LOG_EXIT();
		return -6;
	}

	plain.resize(static_cast<size_t>(len_update) + static_cast<size_t>(len_final));
	EVP_CIPHER_CTX_free(ctx);
	spdlog::debug("aes-256-gcm cipher: \"{}\"", base64(cipher));
	spdlog::debug("aes-256-gcm key:    \"{}\"", base64(key_));
	spdlog::debug("aes-256-gcm iv:     \"{}\"", base64(iv_));
	spdlog::debug("aes-256-gcm plain:  \"{}\"", base64(plain));
	LOG_EXIT();
	return 0;
}