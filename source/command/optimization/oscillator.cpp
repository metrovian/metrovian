#include "command/optimization/oscillator.h"
#include "reconstruction/optimization/oscillator.h"
#include "predefined.h"

void command_oscillator::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("oscillator", "OSCILLATOR optimization")->group("OPTIMIZATION");
	command->add_option("-f, --func", function_, "oscillator-damped function")->required();
	command->add_option("-i, --in", in_, "measured csv");
	command->add_option("-n, --iter", iter_, "iteration max")->default_val<size_t>(1000);
	command->add_option("-e, --eps", eps_, "iteration epsilon")->default_val<double>(1.000E-15);
	command->add_option("-p, --params", params_, "parameters");
	command->callback([this]() { run(); });
	map_.insert(std::make_pair<std::string, oscillator::function>("un-damped", oscillator::function::un_damped));
	map_.insert(std::make_pair<std::string, oscillator::function>("under-damped", oscillator::function::under_damped));
	map_.insert(std::make_pair<std::string, oscillator::function>("over-damped", oscillator::function::over_damped));
	map_.insert(std::make_pair<std::string, oscillator::function>("critical-damped", oscillator::function::critical_damped));
	return;
}

void command_oscillator::run() {
	if (map_.find(function_) != map_.end()) {
		optimization_oscillator engine;
		if (engine.import_function(map_[function_]).length() > 0) {
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
	}

	return;
}