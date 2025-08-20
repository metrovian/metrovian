#include "command_abstract.h"
#include "decryption_aes.h"

int8_t command_abstract::read_binary(const std::string &path, std::vector<uint8_t> &binary) {
	std::ifstream ifs(path, std::ios::binary);
	if (ifs.is_open() == false) {
		return -1;
	}

	binary.clear();
	binary.resize(static_cast<size_t>(std::filesystem::file_size(path)));
	ifs.read(reinterpret_cast<char *>(binary.data()), binary.size());
	if (ifs.fail() == true) {
		return -2;
	}

	return 0;
}

int8_t command_abstract::write_binary(const std::string &path, std::vector<uint8_t> &binary) {
	std::ofstream ofs(path, std::ios::binary);
	if (ofs.is_open() == false) {
		return -1;
	}

	ofs.write(reinterpret_cast<const char *>(binary.data()), binary.size());
	if (ofs.fail() == true) {
		return -2;
	}

	return 0;
}

int8_t command_abstract::read_text(const std::string &path, std::string &text) {
	return 0;
}

int8_t command_abstract::write_text(const std::string &path, std::string &text) {
	return 0;
}

void command_aes256_cbc::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("aes-256-cbc", "AES-256-CBC decryption");
	command->add_option("--key", key, "key base64")->required();
	command->add_option("--iv", iv, "iv base64")->required();
	command->add_option("--in", in, "ciphertext binary")->required();
	command->add_option("--out", out, "plaintext binary")->required();
	command->callback([this]() { run(); });
	return;
}

void command_aes256_cbc::run() {
	decryption_aes256_cbc engine;
	if (engine.setkey(key) == 0) {
		if (engine.setiv(iv) == 0) {
			std::vector<uint8_t> cipher;
			std::vector<uint8_t> plain;
			if (read_binary(in, cipher) == 0) {
				if (engine.decrypt(cipher, plain) == 0) {
					if (write_binary(out, plain) == 0) {
						spdlog::info("AES-256-CBC decryption success");
						return;
					}
				}
			}
		}
	}

	return;
}