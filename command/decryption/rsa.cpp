#include "decryption/rsa.h"
#include "reconstruction/decryption/rsa.h"
#include "predefined.h"

void command_rsa::setup(CLI::App *parent) {
	command_parser_ = parent->add_subcommand("rsa", "RSA decryption")->group("DECRYPTION");
	command_parser_->callback([this]() { run(); });
	setup_subcommand(std::make_unique<command_rsa_private>());
	setup_subcommand(std::make_unique<command_rsa_public>());
	return;
}

void command_rsa::run() {
	if (select_subcommand() == 0) {
		throw CLI::CallForHelp();
	}

	return;
}

void command_rsa_private::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("private", "RSA decryption");
	command->add_option("-p, --pem", private_pem_, "private pem")->required();
	command->add_option("-i, --in", in_, "ciphertext binary")->required();
	command->add_option("-o, --out", out_, "plaintext binary")->required();
	command->add_option("-f, --func", function_, "hash function")->required();
	command->callback([this]() { run(); });
	map_.insert(std::make_pair("sha256", decryption_abstract::sha256));
	map_.insert(std::make_pair("sha512", decryption_abstract::sha512));
	return;
}

void command_rsa_private::run() {
	if (map_.find(function_) != map_.end()) {
		decryption_rsa engine;
		std::string private_key;
		if (read_text(private_pem_, private_key) == 0) {
			if (engine.setkey(private_key) == 0) {
				std::vector<uint8_t> cipher;
				std::vector<uint8_t> plain;
				if (read_binary(in_, cipher) == 0) {
					if (engine.decrypt(map_[function_](cipher), plain) == 0) {
						if (write_binary(out_, plain) == 0) {
							std::cout << engine.base64(plain) << std::endl;
							return;
						}
					}
				}
			}
		}
	}
}

void command_rsa_public::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("public", "RSA-EXPLOIT decryption");
	command->add_option("-i, --in", in_, "public pem")->required();
	command->add_option("-o, --out", out_, "private pem")->required();
	command->add_option("-m, --method", method_, "method")->required();
	command->callback([this]() { run(); });
	map_.insert(std::make_pair<std::string, rsa::attack>("trial", rsa::attack::trial));
	map_.insert(std::make_pair<std::string, rsa::attack>("fermat", rsa::attack::fermat));
	map_.insert(std::make_pair<std::string, rsa::attack>("pollards-rho", rsa::attack::pollards_rho));
	map_.insert(std::make_pair<std::string, rsa::attack>("pollards-p1", rsa::attack::pollards_p1));
	map_.insert(std::make_pair<std::string, rsa::attack>("williams-p1", rsa::attack::williams_p1));
}

void command_rsa_public::run() {
	if (map_.find(method_) != map_.end()) {
		decryption_rsa engine;
		std::string public_key;
		if (read_text(in_, public_key) == 0) {
			if (engine.calckey(public_key, map_[method_]) == 0) {
				std::string pem = engine.pem();
				if (write_text(out_, pem) == 0) {
					std::cout << pem << std::endl;
					return;
				}
			}
		}
	}
}