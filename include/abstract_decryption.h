#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <sstream>
#include <string>
#include <stdint.h>
#include <spdlog/spdlog.h>

class abstract_decryption {
protected: /* stream transform */
	static std::vector<uint8_t> stob(const std::string &chars);
	static std::vector<uint8_t> htob(const std::string &hexs);
	static std::string btos(const std::vector<uint8_t> &bytes);
	static std::string btoh(const std::vector<uint8_t> &bytes);

public: /* abstract interface */
	virtual int8_t decrypt(const std::vector<uint8_t> &cipher, std::vector<uint8_t> &plain) = 0;
	virtual ~abstract_decryption() {}
};