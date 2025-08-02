#include "decryption_abstract.h"

std::vector<uint8_t> decryption_abstract::stob(const std::string &chars) {
	return std::vector<uint8_t>(chars.begin(), chars.end());
}

std::vector<uint8_t> decryption_abstract::htob(const std::string &hexs) {
	spdlog::trace("[enter] {}", __PRETTY_FUNCTION__);
	if (hexs.size() % 2 != 0) {
		spdlog::critical("[invalid arguments] {}", __PRETTY_FUNCTION__);
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return std::vector<uint8_t>();
	}

	std::vector<uint8_t> bytes(hexs.size() / 2);
	for (uint64_t i = 0; i < hexs.size(); i += 2) {
		bytes.push_back(std::stoi(hexs.substr(i, 2), nullptr, 16));
	}

	spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
	return bytes;
}

std::string decryption_abstract::btos(const std::vector<uint8_t> &bytes) {
	return std::string(bytes.begin(), bytes.end());
}

std::string decryption_abstract::btoh(const std::vector<uint8_t> &bytes) {
	spdlog::trace("[enter] {}", __PRETTY_FUNCTION__);
	std::ostringstream oss;
	oss << std::hex << std::setfill('0');
	for (uint8_t byte : bytes) {
		oss << std::setw(2) << static_cast<int32_t>(byte);
	}

	spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
	return oss.str();
}

int8_t decryption_abstract::decrypt(const std::vector<uint8_t> &cipher, std::string &plain) {
	spdlog::trace("[enter] {}", __PRETTY_FUNCTION__);
	std::vector<uint8_t> bytes;
	int8_t retcode = decrypt(cipher, bytes);
	plain = btos(bytes);
	spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
	return retcode;
}

int8_t decryption_abstract::decrypt(const std::string &cipher, std::string &plain) {
	spdlog::trace("[enter] {}", __PRETTY_FUNCTION__);
	std::vector<uint8_t> chars = stob(cipher);
	std::vector<uint8_t> bytes;
	int8_t retcode = decrypt(chars, bytes);
	plain = btos(bytes);
	spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
	return retcode;
}

int8_t decryption_abstract::decrypt(const std::string &cipher, std::vector<uint8_t> &plain) {
	spdlog::trace("[enter] {}", __PRETTY_FUNCTION__);
	std::vector<uint8_t> chars = stob(cipher);
	int8_t retcode = decrypt(chars, plain);
	spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
	return retcode;
}
