#include "command/operation/abstract.h"
#include "core/operation/fourier.h"

void command_fourier::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("fourier")->group("OPERATION");
	command->add_option("-i, --in", in_, "measured wav");
	command->add_option("-o, --out", out_, "return csv")->required();
	command->callback([this]() { run(); });
	return;
}

void command_fourier::run() {
	operation_fft engine;
	Eigen::VectorXd sample;
	Eigen::VectorXd spectrum;
	Eigen::VectorXd frequency;
	int sample_rate = read_audio(in_, sample);
	if (sample_rate > 0) {
		if (engine.operate(sample, spectrum) == 0) {
			int half = spectrum.size() / 2 + 1;
			spectrum = spectrum.head(half);
			frequency = Eigen::VectorXd::LinSpaced(half, 0.000E+0, sample_rate / 2.000E+0);
			write_vector(out_, frequency, spectrum, ',');
		}
	}

	return;
}