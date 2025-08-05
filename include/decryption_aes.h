#pragma once
#include "decryption_abstract.h"

class decryption_aes : public decryption_abstract {
protected: /* parameter */
	std::vector<uint8_t> key_;
	std::vector<uint8_t> iv_;

public: /* setter */
	int8_t setkey(const std::vector<uint8_t> &key);
	int8_t setiv(const std::vector<uint8_t> &iv);
	int8_t setkey(const std::string &key);
	int8_t setiv(const std::string &iv);

protected: /* abstract */
	virtual size_t key_size() = 0;
	virtual size_t iv_size() = 0;
};

class decryption_aes256 : public decryption_aes {
protected: /* abstract */
	virtual size_t key_size() override final { return static_cast<size_t>(32); }
	virtual size_t iv_size() override { return static_cast<size_t>(16); }
};

class decryption_aes256_cbc : public decryption_aes256 {
protected: /* abstract */
	virtual int8_t decryption(const std::vector<uint8_t> &cipher, std::vector<uint8_t> &plain) override final;
};

class decryption_aes256_ctr : public decryption_aes256 {
protected: /* abstract */
	virtual int8_t decryption(const std::vector<uint8_t> &cipher, std::vector<uint8_t> &plain) override final;
};