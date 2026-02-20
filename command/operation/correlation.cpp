#include "command/operation/abstract.h"
#include "core/operation/correlation.h"

void command_correlation::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("correlation")->group("OPERATION");
	command->add_option("-i, --in", in_, "measured wav");
	command->add_option("-o, --out", out_, "return csv")->required();
	command->callback([this]() { run(); });
	return;
}

void command_correlation::run() {
	operation_correlation engine;
	Eigen::VectorXd sample;
	Eigen::VectorXd acorr;
	Eigen::VectorXd time;
	int sample_rate = read_audio(in_, sample);
	if (sample_rate > 0) {
		acorr = engine.operate(sample);
		time = Eigen::VectorXd::LinSpaced(acorr.size(), 0.000E+0, (acorr.size() - 1));
		time /= static_cast<double>(sample_rate);
		write_vector(out_, time, acorr, ',');
	}

	return;
}