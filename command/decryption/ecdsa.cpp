#include "decryption/ecdsa.h"
#include "reconstruction/decryption/ecdsa.h"
#include "predefined.h"

void command_ecdsa::setup(CLI::App *parent) {
	command_parser_ = parent->add_subcommand("ecdsa", "ECDSA decryption")->group("DECRYPTION");
	command_parser_->callback([this]() { run(); });
	setup_subcommand(std::make_unique<command_ecdsa_private>());
	setup_subcommand(std::make_unique<command_ecdsa_public>());
	return;
}

void command_ecdsa::run() {
	if (select_subcommand() == 0) {
		throw CLI::CallForHelp();
	}

	return;
}

void command_ecdsa_private::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("private", "ECDSA decryption")->group("DECRYPTION");
	command->add_option("-p, --pem", private_pem_, "private pem")->required();
	command->add_option("-i, --in", in_, "ciphertext binary")->required();
	command->add_option("-o, --out", out_, "plaintext binary")->required();
	command->callback([this]() { run(); });
	return;
}

void command_ecdsa_private::run() {
	decryption_ecdsa engine;
	std::string private_key;
	if (read_text(private_pem_, private_key) == 0) {
		if (engine.setkey(private_key) == 0) {
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
}

void command_ecdsa_public::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("public", "ECDSA-EXPLOIT decryption");
	command->add_option("-p, --pem", public_pem_, "public pem")->required();
	command->add_option("-m, --method", method_, "attack method")->required();
	command->add_option("-i, --in", in_, "ciphertext binary");
	command->add_option("-o, --out", out_, "plaintext binary")->required();
	command->callback([this]() { run(); });
	map_.insert(std::make_pair<std::string, ecdsa::attack>("trial", ecdsa::attack::trial));
}

void command_ecdsa_public::run() {
	if (map_.find(method_) != map_.end()) {
		decryption_ecdsa engine;
		std::string public_key;
		if (read_text(public_pem_, public_key) == 0) {
			if (engine.calckey(public_key, map_[method_]) == 0) {
				std::vector<uint8_t> cipher;
				std::vector<uint8_t> plain;
				if (in_.length() > 0) {
					if (read_binary(in_, cipher) == 0) {
						if (engine.decrypt(cipher, plain) == 0) {
							if (write_binary(out_, plain) == 0) {
								std::cout << engine.base64(plain) << std::endl;
								return;
							}
						}
					}
				} else {
					std::string pem = engine.pem();
					if (write_text(out_, pem) == 0) {
						std::cout << pem << std::endl;
						return;
					}
				}
			}
		}
	}
}