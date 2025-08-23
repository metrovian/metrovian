#include "command_abstract.h"
#include "predefined.h"

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

int8_t command_abstract::read_vector(const std::string &path, Eigen::VectorXd &domain, Eigen::VectorXd &range, char delimiter) {
	std::ifstream ifs(path);
	if (ifs.is_open() == false) {
		return -1;
	}

	std::vector<double> read_domain;
	std::vector<double> read_range;
	std::string line;
	while (std::getline(ifs, line).fail() == false) {
		std::istringstream iss(line);
		std::string token1;
		std::string token2;
		if (std::getline(iss, token1, delimiter).fail() == true) {
			continue;
		} else if (std::getline(iss, token2, delimiter).fail() == true) {
			continue;
		}

		read_domain.push_back(std::stod(token1));
		read_range.push_back(std::stod(token2));
	}

	domain = Eigen::Map<Eigen::VectorXd>(read_domain.data(), read_domain.size());
	range = Eigen::Map<Eigen::VectorXd>(read_range.data(), read_range.size());
	return 0;
}

int8_t command_abstract::read_write(const std::string &path, Eigen::VectorXd &domain, Eigen::VectorXd &range, char delimiter) {
	std::ofstream ofs(path);
	if (ofs.is_open() == false) {
		return -1;
	} else if (domain.size() != range.size()) {
		return -2;
	}

	for (size_t i = 0; i < domain.size(); ++i) {
		ofs << domain[i] << delimiter << range[i] << std::endl;
	}

	if (ofs.fail() == true) {
		return -3;
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
						std::cout << engine.base64(plain) << std::endl;
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
						std::cout << engine.base64(plain) << std::endl;
						return;
					}
				}
			}
		}
	}
}

void command_rsa_attack::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("rsa-attack", "RSA-EXPLOIT decryption");
	command->add_option("-p, --pem", public_pem_, "public pem")->required();
	command->add_option("-m, --method", method_, "attack method")->required();
	command->add_option("-i, --in", in_, "ciphertext binary")->required();
	command->add_option("-o, --out", out_, "plaintext binary")->required();
	command->callback([this]() { run(); });
	map_.insert(std::make_pair<std::string, rsa::attack>("fermat", rsa::attack::fermat));
	map_.insert(std::make_pair<std::string, rsa::attack>("pollards-rho", rsa::attack::pollards_rho));
	map_.insert(std::make_pair<std::string, rsa::attack>("pollards-p1", rsa::attack::pollards_p1));
	map_.insert(std::make_pair<std::string, rsa::attack>("williams-p1", rsa::attack::williams_p1));
}

void command_rsa_attack::run() {
	if (map_.find(method_) != map_.end()) {
		decryption_rsa engine;
		std::string public_key;
		if (read_text(public_pem_, public_key) == 0) {
			if (engine.calckey(public_key, map_[method_]) == 0) {
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
}

void command_avcodec::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("avcodec", "FFMPEG decompression");
	command->add_option("-i, --in", in_, "encoded audio")->required();
	command->callback([this]() { run(); });
	return;
}

void command_avcodec::run() {
	decompression_avcodec engine;
	engine.decompress(in_);
	return;
}

void command_user::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("user", "USER optimization");
	command->add_option("-f, --func", function_, "user-defined function")->required();
	command->add_option("-i, --in", in_, "measured csv")->required();
	command->add_option("-n, --iter", iter_, "iteration max")->default_val<size_t>(1000);
	command->add_option("-e, --eps", eps_, "iteration eps")->default_val<double>(1.000E-15);
	command->callback([this]() { run(); });
	return;
}

void command_user::run() {
	optimization_user engine;
	if (engine.import_function(function_).length() > 0) {
		Eigen::VectorXd domain;
		Eigen::VectorXd range;
		if (read_vector(in_, domain, range, ',') == 0) {
			if (engine.calibrate(domain, range, iter_, eps_).norm() > 0) {
				std::cout << engine.export_parameters().transpose() << std::endl;
			}
		}
	}

	return;
}