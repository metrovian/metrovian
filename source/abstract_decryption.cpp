#include "abstract_decryption.h"

std::vector<uint8_t> abstract_decryption::stob(const std::string &chars) {
	return std::vector<uint8_t>(chars.begin(), chars.end());
}

std::vector<uint8_t> abstract_decryption::htob(const std::string &hexs) {
	spdlog::trace("[abstract_decryption::htob] enter");
	if (hexs.size() % 2 != 0) {
		spdlog::critical("[abstract_decryption::htob] invalid arguments: hexs.size() % 2 != 0");
		spdlog::trace("[abstract_decryption::htob] exit");
		return std::vector<uint8_t>();
	}

	std::vector<uint8_t> bytes(hexs.size() / 2);
	for (uint64_t i = 0; i < hexs.size(); i += 2) {
		bytes.push_back(std::stoi(hexs.substr(i, 2), nullptr, 16));
	}

	spdlog::trace("[abstract_decryption::htob] exit");
	return bytes;
}

std::string abstract_decryption::btos(const std::vector<uint8_t> &bytes) {
	return std::string(bytes.begin(), bytes.end());
}

std::string abstract_decryption::btoh(const std::vector<uint8_t> &bytes) {
	spdlog::trace("[abstract_decryption::btoh] enter");
	std::ostringstream oss;
	oss << std::hex << std::setfill('0');
	for (uint8_t byte : bytes) {
		oss << std::setw(2) << static_cast<int32_t>(byte);
	}

	spdlog::trace("[abstract_decryption::btoh] exit");
	return oss.str();
}