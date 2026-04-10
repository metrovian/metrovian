#include "command/optimization/abstract.h"
#include "core/optimization/rational.h"
#include "core/predefined.h"

void command_rational::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("rational")->group("OPTIMIZATION");
	command->add_option("-f, --func", model_, "rational-approximated function")->required()->expected(2);
	command->add_option("-i, --in", in_, "measured csv");
	command->add_option("-n, --iter", iter_, "iteration max")->default_val<size_t>(1000);
	command->add_option("-e, --eps", eps_, "iteration epsilon")->default_val<double>(1.000E-15);
	command->add_option("-p, --params", params_, "parameters");
	command->callback([this]() { run(); });
	return;
}

void command_rational::run() {
	optimization_rational engine;
	if (engine.import_function(model_).length() > 0) {
		calibrate(&engine);
	}

	return;
}