#include "decryption/aes.h"
#include "reconstruction/decryption/aes.h"
#include "predefined.h"

void command_aes256_cbc::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("aes-256-cbc", "AES-256-CBC decryption")->group("DECRYPTION");
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
						std::cout << engine.base64(plain) << std::endl;
						return;
					}
				}
			}
		}
	}

	return;
}

void command_aes256_ctr::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("aes-256-ctr", "AES-256-CTR decryption")->group("DECRYPTION");
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
						std::cout << engine.base64(plain) << std::endl;
						return;
					}
				}
			}
		}
	}

	return;
}

void command_aes256_gcm::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("aes-256-gcm", "AES-256-GCM decryption")->group("DECRYPTION");
	command->add_option("-k, --key", key_, "key base64")->required();
	command->add_option("-v, --iv", iv_, "iv base64")->required();
	command->add_option("-i, --in", in_, "ciphertext binary")->required();
	command->add_option("-o, --out", out_, "plaintext binary")->required();
	command->callback([this]() { run(); });
	return;
}

void command_aes256_gcm::run() {
	decryption_aes256_gcm engine;
	if (engine.setkey(key_) == 0) {
		if (engine.setiv(iv_) == 0) {
			std::vector<uint8_t> cipher;
			std::vector<uint8_t> plain;
			if (read_binary(in_, cipher) == 0) {
				if (engine.decrypt(cipher, plain) == 0) {
					if (write_binary(out_, plain) == 0) {
						std::cout << engine.base64(plain) << std::endl;
						return;
					}
				}
			}
		}
	}

	return;
}