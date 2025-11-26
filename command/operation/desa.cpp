#include "command/operation/desa.h"
#include "core/operation/desa.h"

void command_desa::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("desa", "DESA operation")->group("OPERATION");
	command->add_option("-i, --in", in_, "measured csv");
	command->add_option("-o, --out", out_, "return csv")->required();
	command->callback([this]() { run(); });
	return;
}

void command_desa::run() {
	operation_desa2 engine;
	Eigen::VectorXd sample;
	Eigen::VectorXd iamp;
	if (read_vector(in_, sample) == 0) {
		if (engine.operate(sample, iamp) == 0) {
			write_vector(out_, iamp);
		}
	}

	return;
}