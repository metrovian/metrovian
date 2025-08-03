#include "decryption_base.h"

int8_t decryption_base64::decryption(const std::vector<uint8_t> &cipher, std::vector<uint8_t> &plain) {
	spdlog::trace("[enter] {}", __PRETTY_FUNCTION__);
	BIO *base64 = BIO_new(BIO_f_base64());
	BIO *bio = BIO_new_mem_buf(cipher.data(), static_cast<int32_t>(cipher.size()));
	bio = BIO_push(base64, bio);
	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
	plain.clear();
	plain.resize((cipher.size() * 3) / 4);
	int32_t BIO_retcode = BIO_read(bio, plain.data(), static_cast<int32_t>(plain.size()));
	BIO_free_all(bio);
	if (BIO_retcode == 0) {
		spdlog::error("base64 decryption failed: \"{}\"", btos(cipher));
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	} else if (BIO_retcode < 0) {
		spdlog::critical("[invalid implementation] {}", __PRETTY_FUNCTION__);
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return -1;
	}

	plain.resize(static_cast<uint64_t>(BIO_retcode));
	spdlog::info("base64 cipher: \"{}\"", btos(cipher));
	spdlog::info("base64 plain: \"{}\"", btos(plain));
	spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
	return 0;
}