#include "command/operation/abstract.h"
#include "core/operation/fft.h"

void command_fft::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("fourier", "fourier operation")->group("OPERATION");
	command->add_option("-i, --in", in_, "measured csv");
	command->add_option("-o, --out", out_, "return csv")->required();
	command->callback([this]() { run(); });
	return;
}

void command_fft::run() {
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