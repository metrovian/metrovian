#include "command/user.h"
#include "optimization/user.h"
#include "predefined.h"

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