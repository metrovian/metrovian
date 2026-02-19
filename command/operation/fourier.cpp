#include "command/operation/abstract.h"
#include "core/operation/fourier.h"

void command_fourier::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("fourier")->group("OPERATION");
	command->add_option("-i, --in", in_, "measured csv");
	command->add_option("-o, --out", out_, "return csv")->required();
	command->callback([this]() { run(); });
	return;
}

void command_fourier::run() {
	operation_fft engine;
	Eigen::VectorXd sample;
	Eigen::VectorXd spectrum;
	if (read_vector(in_, sample) == 0) {
		if (engine.operate(sample, spectrum) == 0) {
			write_vector(out_, spectrum);
		}
	}

	return;
}