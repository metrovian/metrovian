#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <sstream>
#include <string>
#include <stdint.h>
#include <spdlog/spdlog.h>
#include <openssl/core_names.h>
#include <openssl/param_build.h>
#include <openssl/bio.h>
#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/buffer.h>

class decryption_abstract {
public: /* stream transform */
	static std::vector<uint8_t> base64(const std::string &chars);
	static std::string base64(const std::vector<uint8_t> &bytes);

public: /* overload */
	int8_t decrypt(const std::vector<uint8_t> &cipher, std::vector<uint8_t> &plain);
	int8_t decrypt(const std::vector<uint8_t> &cipher, std::string &plain);
	int8_t decrypt(const std::string &cipher, std::string &plain);
	int8_t decrypt(const std::string &cipher, std::vector<uint8_t> &plain);

public: /* abstract */
	virtual ~decryption_abstract() {}

protected: /* abstract */
	virtual int8_t decryption(const std::vector<uint8_t> &cipher, std::vector<uint8_t> &plain) = 0;
};