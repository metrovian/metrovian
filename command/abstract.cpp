#include "command/abstract.h"
#include "core/predefined.h"

#include <csignal>

void command_abstract::handle_setup(const std::function<void(void)> handler) {
	handler_ = handler;
	if (handler_ != nullptr) {
		std::signal(SIGTERM, command_abstract::handle_terminate);
		std::signal(SIGINT, command_abstract::handle_terminate);
	}

	return;
}

void command_abstract::handle_terminate(int) {
	if (handler_ != nullptr) {
		handler_();
	}

	return;
}

int command_abstract::read_binary(const std::string &path, std::vector<uint8_t> &binary) {
	std::ifstream ifs(path, std::ios::binary);
	if (ifs.is_open() == false) {
		return -1;
	}

	binary.resize(static_cast<size_t>(std::filesystem::file_size(path)));
	ifs.read(reinterpret_cast<char *>(binary.data()), binary.size());
	if (ifs.fail() == true) {
		return -2;
	}

	return 0;
}

int command_abstract::read_audio(const std::string &path, Eigen::VectorXd &range) {
	std::vector<uint8_t> binary;
	if (read_binary(path, binary) != 0) {
		return -1;
	} else if (binary.size() < 44) {
		return -2;
	}

	auto ptr = binary.data();
	if (std::memcmp(ptr, "RIFF", 4) != 0) {
		return -3;
	} else if (std::memcmp(ptr + 8, "WAVE", 4) != 0) {
		return -4;
	}

	const uint8_t *cptr = nullptr;
	const uint8_t *dptr = nullptr;
	uint32_t csize = 0;
	uint32_t dsize = 0;
	uint64_t unit = 0;
	uint16_t format = 0;
	uint16_t channel = 0;
	uint32_t sample_rate = 0;
	uint16_t bps = 0;
	uint64_t ofs = 12;
	while (ofs + 8 <= binary.size()) {
		cptr = ptr + ofs;
		csize = *reinterpret_cast<const uint32_t *>(cptr + 4);
		if (std::memcmp(cptr, "fmt ", 4) == 0) {
			format = *reinterpret_cast<const uint16_t *>(cptr + 8);
			channel = *reinterpret_cast<const uint16_t *>(cptr + 10);
			sample_rate = *reinterpret_cast<const uint32_t *>(cptr + 12);
			bps = *reinterpret_cast<const uint16_t *>(cptr + 22);
			if (format != 1) {
				return -5;
			} else if (bps != 16) {
				return -6;
			}
		} else if (std::memcmp(cptr, "data", 4) == 0) {
			dptr = cptr + 8;
			dsize = csize;
			unit = channel * (bps / 8);
			break;
		}

		ofs += static_cast<uint64_t>(csize + 8);
	}

	if (dptr == nullptr) {
		return -7;
	}

	range.resize(dsize / unit);
	for (Eigen::Index i = 0; i < range.size(); ++i) {
		range[i] = *reinterpret_cast<const int16_t *>(dptr + i * unit);
	}

	range /= static_cast<double>(INT16_MAX + 1);
	return static_cast<int>(sample_rate);
}

int command_abstract::read_vector(const std::string &path, Eigen::VectorXd &domain, Eigen::VectorXd &range, char delimiter) {
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

int command_abstract::write_vector(const std::string &path, Eigen::VectorXd &domain, Eigen::VectorXd &range, char delimiter) {
	std::ofstream ofs(path);
	if (ofs.is_open() == false) {
		return -1;
	} else if (domain.size() != range.size()) {
		return -2;
	}

	for (Eigen::Index i = 0; i < domain.size(); ++i) {
		ofs << domain[i] << delimiter << range[i] << std::endl;
	}

	if (ofs.fail() == true) {
		return -3;
	}

	return 0;
}

int command_abstract::setup_subcommand(std::unique_ptr<command_abstract> command) {
	command->setup(command_parser_);
	commands_.emplace_back(std::move(command));
	return 0;
}

int command_abstract::select_subcommand() {
	for (auto &command : command_parser_->get_subcommands()) {
		if (command->parsed() == true) {
			return 1;
		}
	}

	return 0;
}
