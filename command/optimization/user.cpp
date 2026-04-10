#include "command/optimization/abstract.h"
#include "core/optimization/user.h"
#include "core/predefined.h"

void command_user::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("user")->group("OPTIMIZATION");
	command->add_option("-m, --model", func_, "user-defined model")->required();
	command->add_option("-i, --in", in_, "measured csv");
	command->add_option("-n, --iter", iter_, "iteration max")->default_val<size_t>(1000);
	command->add_option("-e, --eps", eps_, "iteration epsilon")->default_val<double>(1.000E-15);
	command->add_option("-p, --params", params_, "parameters");
	command->callback([this]() { run(); });
	return;
}

void command_user::run() {
	optimization_user engine;
	if (engine.import_function(func_).length() > 0) {
		calibrate(&engine);
	}

	return;
}

void command_user::calibrate(optimization_abstract *engine) {
	Eigen::VectorXd domain;
	Eigen::VectorXd range;
	Eigen::VectorXd params = engine->export_parameters();
	for (size_t i = 0; i < std::min(static_cast<size_t>(params.size()), params_.size()); ++i) {
		params[i] = params_[i];
	}

	engine->import_parameters(params);
	if (read_vector(in_, domain, range, ',') == 0) {
		double error = engine->calibrate(domain, range, iter_, eps_).norm() / std::sqrt(domain.size());
		if (error >= 0) {
			params = engine->export_parameters();
			std::cout << "error: " << error << std::endl;
			for (Eigen::Index i = 0; i < params.size(); ++i) {
				std::cout
				    << std::noshowpos
				    << "c" << i << ": " << std::showpos
				    << params[i] << std::endl;
			}
		}
	} else {
		std::cout << engine->export_function() << std::endl;
	}
}