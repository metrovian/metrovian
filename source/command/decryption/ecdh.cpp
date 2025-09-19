#include "command/decryption/ecdh.h"
#include "reconstruction/decryption/ecdh.h"
#include "predefined.h"

void command_ecdh::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("ecdh", "ECDH decryption")->group("DECRYPTION");
	command->add_option("-p, --pem", private_pem_, "private pem")->required();
	command->add_option("-i, --in", in_, "ciphertext binary")->required();
	command->add_option("-o, --out", out_, "plaintext binary")->required();
	command->callback([this]() { run(); });
	return;
}

void command_ecdh::run() {
	decryption_ecdh engine;
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