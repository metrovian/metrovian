#include "decryption_abstract.h"

std::vector<uint8_t> decryption_abstract::stob(const std::string &chars) {
	return std::vector<uint8_t>(chars.begin(), chars.end());
}

std::vector<uint8_t> decryption_abstract::htob(const std::string &hexs) {
	spdlog::trace("[decryption_abstract::htob] enter");
	if (hexs.size() % 2 != 0) {
		spdlog::critical("[decryption_abstract::htob] invalid arguments: hexs.size() % 2 != 0");
		spdlog::trace("[decryption_abstract::htob] exit");
		return std::vector<uint8_t>();
	}

	std::vector<uint8_t> bytes(hexs.size() / 2);
	for (uint64_t i = 0; i < hexs.size(); i += 2) {
		bytes.push_back(std::stoi(hexs.substr(i, 2), nullptr, 16));
	}

	spdlog::trace("[decryption_abstract::htob] exit");
	return bytes;
}

std::string decryption_abstract::btos(const std::vector<uint8_t> &bytes) {
	return std::string(bytes.begin(), bytes.end());
}

std::string decryption_abstract::btoh(const std::vector<uint8_t> &bytes) {
	spdlog::trace("[decryption_abstract::btoh] enter");
	std::ostringstream oss;
	oss << std::hex << std::setfill('0');
	for (uint8_t byte : bytes) {
		oss << std::setw(2) << static_cast<int32_t>(byte);
	}

	spdlog::trace("[decryption_abstract::btoh] exit");
	return oss.str();
}