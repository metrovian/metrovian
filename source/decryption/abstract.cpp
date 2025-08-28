#include "decryption/abstract.h"

std::vector<uint8_t> decryption_abstract::base64(const std::string &chars) {
	BIO *base64 = BIO_new(BIO_f_base64());
	BIO *bio = BIO_new_mem_buf(chars.data(), static_cast<int32_t>(chars.size()));
	bio = BIO_push(base64, bio);
	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
	std::vector<uint8_t> decoded((chars.length() * 3) / 4);
	int32_t len_read = BIO_read(bio, decoded.data(), static_cast<int32_t>(decoded.size()));
	BIO_free_all(bio);
	if (len_read > 0) {
		decoded.resize(static_cast<size_t>(len_read));
		return decoded;
	}

	return std::vector<uint8_t>();
}

std::string decryption_abstract::base64(const std::vector<uint8_t> &bytes) {
	BIO *base64 = BIO_new(BIO_f_base64());
	BIO *bio = BIO_new(BIO_s_mem());
	bio = BIO_push(base64, bio);
	BIO_set_flags(base64, BIO_FLAGS_BASE64_NO_NL);
	BIO_write(bio, bytes.data(), static_cast<int32_t>(bytes.size()));
	BIO_flush(bio);
	BUF_MEM *buf_mem = nullptr;
	BIO_get_mem_ptr(bio, &buf_mem);
	std::string encoded(buf_mem->data, buf_mem->length);
	BIO_free_all(bio);
	return encoded;
}

std::vector<uint8_t> decryption_abstract::hash(const std::vector<uint8_t> &bytes, const EVP_MD *algorithm) {
	EVP_MD_CTX *ctx = EVP_MD_CTX_new();
	if (ctx == nullptr) {
		return std::vector<uint8_t>();
	}

	if (EVP_DigestInit_ex(ctx, algorithm, nullptr) != 1) {
		EVP_MD_CTX_free(ctx);
		return std::vector<uint8_t>();
	}

	if (EVP_DigestUpdate(ctx, bytes.data(), bytes.size()) != 1) {
		EVP_MD_CTX_free(ctx);
		return std::vector<uint8_t>();
	}

	std::vector<uint8_t> digest(EVP_MD_size(algorithm));
	uint32_t len_final = 0;
	if (EVP_DigestFinal_ex(ctx, digest.data(), &len_final) != 1) {
		EVP_MD_CTX_free(ctx);
		return std::vector<uint8_t>();
	}

	EVP_MD_CTX_free(ctx);
	digest.resize(static_cast<size_t>(len_final));
	return digest;
}

std::vector<uint8_t> decryption_abstract::md5(const std::vector<uint8_t> &bytes) {
	return hash(bytes, EVP_md5());
}

std::vector<uint8_t> decryption_abstract::sha256(const std::vector<uint8_t> &bytes) {
	return hash(bytes, EVP_sha256());
}

std::vector<uint8_t> decryption_abstract::sha512(const std::vector<uint8_t> &bytes) {
	return hash(bytes, EVP_sha512());
}

int8_t decryption_abstract::decrypt(const std::vector<uint8_t> &cipher, std::vector<uint8_t> &plain) {
	return decryption(cipher, plain);
}

int8_t decryption_abstract::decrypt(const std::vector<uint8_t> &cipher, std::string &plain) {
	std::vector<uint8_t> bytes_plain;
	int8_t retcode = decryption(cipher, bytes_plain);
	plain = std::move(base64(bytes_plain));
	return retcode;
}

int8_t decryption_abstract::decrypt(const std::string &cipher, std::string &plain) {
	std::vector<uint8_t> bytes_cipher = base64(cipher);
	std::vector<uint8_t> bytes_plain;
	int8_t retcode = decryption(bytes_cipher, bytes_plain);
	plain = std::move(base64(bytes_plain));
	return retcode;
}

int8_t decryption_abstract::decrypt(const std::string &cipher, std::vector<uint8_t> &plain) {
	std::vector<uint8_t> bytes_cipher = base64(cipher);
	return decryption(bytes_cipher, plain);
}
