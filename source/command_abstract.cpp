#include "command_abstract.h"
#include "decryption_aes.h"
#include "decryption_rsa.h"
#include "decompression_avcodec.h"

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
	std::ifstream ifs(path);
	if (ifs.is_open() == false) {
		return -1;
	}

	std::ostringstream oss;
	oss << ifs.rdbuf();
	if (ifs.fail() == true) {
		return -2;
	}

	text = oss.str();
	return 0;
}

int8_t command_abstract::write_text(const std::string &path, std::string &text) {
	std::ofstream ofs(path);
	if (ofs.is_open() == false) {
		return -1;
	}

	ofs << text;
	if (ofs.fail() == true) {
		return -2;
	}

	return 0;
}

void command_aes256_cbc::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("aes-256-cbc", "AES-256-CBC decryption");
	command->add_option("-k, --key", key_, "key base64")->required();
	command->add_option("-v, --iv", iv_, "iv base64")->required();
	command->add_option("-i, --in", in_, "ciphertext binary")->required();
	command->add_option("-o, --out", out_, "plaintext binary")->required();
	command->callback([this]() { run(); });
	return;
}

void command_aes256_cbc::run() {
	decryption_aes256_cbc engine;
	if (engine.setkey(key_) == 0) {
		if (engine.setiv(iv_) == 0) {
			std::vector<uint8_t> cipher;
			std::vector<uint8_t> plain;
			if (read_binary(in_, cipher) == 0) {
				if (engine.decrypt(cipher, plain) == 0) {
					if (write_binary(out_, plain) == 0) {
						spdlog::info("AES-256-CBC decryption success");
						return;
					}
				}
			}
		}
	}

	return;
}

void command_aes256_ctr::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("aes-256-ctr", "AES-256-CTR decryption");
	command->add_option("-k, --key", key_, "key base64")->required();
	command->add_option("-v, --iv", iv_, "iv base64")->required();
	command->add_option("-i, --in", in_, "ciphertext binary")->required();
	command->add_option("-o, --out", out_, "plaintext binary")->required();
	command->callback([this]() { run(); });
	return;
}

void command_aes256_ctr::run() {
	decryption_aes256_ctr engine;
	if (engine.setkey(key_) == 0) {
		if (engine.setiv(iv_) == 0) {
			std::vector<uint8_t> cipher;
			std::vector<uint8_t> plain;
			if (read_binary(in_, cipher) == 0) {
				if (engine.decrypt(cipher, plain) == 0) {
					if (write_binary(out_, plain) == 0) {
						spdlog::info("AES-256-CTR decryption success");
						return;
					}
				}
			}
		}
	}

	return;
}

void command_rsa::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("rsa", "RSA decryption");
	command->add_option("-p, --pem", private_pem_, "private pem")->required();
	command->add_option("-i, --in", in_, "ciphertext binary")->required();
	command->add_option("-o, --out", out_, "plaintext binary")->required();
	command->callback([this]() { run(); });
	return;
}

void command_rsa::run() {
	decryption_rsa engine;
	std::string private_key;
	if (read_text(private_pem_, private_key) == 0) {
		if (engine.setkey(private_key) == 0) {
			std::vector<uint8_t> cipher;
			std::vector<uint8_t> plain;
			if (read_binary(in_, cipher) == 0) {
				if (engine.decrypt(cipher, plain) == 0) {
					if (write_binary(out_, plain) == 0) {
						spdlog::info("RSA decryption success");
						return;
					}
				}
			}
		}
	}
}

void command_avcodec::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("avcodec", "FFMPEG decompression");
	command->add_option("-i, --in", path_, "encoded audio")->required();
	command->callback([this]() { run(); });
	return;
}

void command_avcodec::run() {
	decompression_avcodec engine;
	engine.decompress(path_);
	return;
}