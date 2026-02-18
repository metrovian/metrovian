#include "command/operation/abstract.h"
#include "core/operation/corr.h"

void command_corr::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("correlation")->group("OPERATION");
	command->add_option("-i, --in", in_, "measured csv");
	command->add_option("-o, --out", out_, "return csv")->required();
	command->callback([this]() { run(); });
	return;
}

void command_corr::run() {
	operation_xcorr engine;
	if (in_.size() == 2) {
		Eigen::VectorXd sample1;
		Eigen::VectorXd sample2;
		if (read_vector(in_[0], sample1) == 0 &&
		    read_vector(in_[1], sample2) == 0) {
			Eigen::VectorXd xcorr = engine.operate(sample1, sample2);
			write_vector(out_, xcorr);
		}
	} else if (in_.size() == 1) {
		Eigen::VectorXd sample;
		if (read_vector(in_[0], sample) == 0) {
			Eigen::VectorXd acorr = engine.operate(sample);
			write_vector(out_, acorr);
		}
	}

	return;
}