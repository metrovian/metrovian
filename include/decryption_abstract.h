#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <sstream>
#include <string>
#include <stdint.h>
#include <spdlog/spdlog.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

class decryption_abstract {
protected: /* stream transform */
	static std::vector<uint8_t> stob(const std::string &chars);
	static std::vector<uint8_t> htob(const std::string &hexs);
	static std::string btos(const std::vector<uint8_t> &bytes);
	static std::string btoh(const std::vector<uint8_t> &bytes);

public: /* overload */
	int8_t decrypt(const std::vector<uint8_t> &cipher, std::string &plain);
	int8_t decrypt(const std::string &cipher, std::string &plain);
	int8_t decrypt(const std::string &cipher, std::vector<uint8_t> &plain);

public: /* abstract */
	virtual int8_t decrypt(const std::vector<uint8_t> &cipher, std::vector<uint8_t> &plain) = 0;
	virtual ~decryption_abstract() {}
};