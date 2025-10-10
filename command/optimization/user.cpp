#include "optimization/user.h"
#include "reconstruction/optimization/user.h"
#include "predefined.h"

void command_user::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("user", "USER optimization")->group("OPTIMIZATION");
	command->add_option("-f, --func", function_, "user-defined function")->required();
	command->add_option("-i, --in", in_, "measured csv");
	command->add_option("-n, --iter", iter_, "iteration max")->default_val<size_t>(1000);
	command->add_option("-e, --eps", eps_, "iteration epsilon")->default_val<double>(1.000E-15);
	command->add_option("-p, --params", params_, "parameters");
	command->callback([this]() { run(); });
	return;
}

void command_user::run() {
	optimization_user engine;
	if (engine.import_function(function_).length() > 0) {
		Eigen::VectorXd domain;
		Eigen::VectorXd range;
		Eigen::VectorXd params = engine.export_parameters();
		for (size_t i = 0; i < std::min(static_cast<size_t>(params.size()), params_.size()); ++i) {
			params[i] = params_[i];
		}

		engine.import_parameters(params);
		if (read_vector(in_, domain, range, ',') == 0) {
			if (engine.calibrate(domain, range, iter_, eps_).norm() >= 0) {
				std::cout << engine.export_parameters().transpose() << std::endl;
			}
		} else {
			std::cout << engine.export_function() << std::endl;
		}
	}

	return;
}