#include "command/oscillator.h"
#include "optimization/oscillator.h"
#include "predefined.h"

void command_oscillator::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("oscillator", "OSCILLATOR optimization");
	command->add_option("-f, --func", function_, "oscillator function")->required();
	command->add_option("-i, --in", in_, "measured csv")->required();
	command->add_option("-n, --iter", iter_, "iteration max")->default_val<size_t>(1000);
	command->add_option("-e, --eps", eps_, "iteration eps")->default_val<double>(1.000E-15);
	command->callback([this]() { run(); });
	map_.insert(std::make_pair<std::string, oscillator::motion>("harmonic", oscillator::motion::harmonic));
	return;
}

void command_oscillator::run() {
	optimization_oscillator engine;
	if (engine.import_function(map_[function_]).length() > 0) {
		Eigen::VectorXd domain;
		Eigen::VectorXd range;
		if (read_vector(in_, domain, range, ',') == 0) {
			if (engine.calibrate(domain, range, iter_, eps_).norm() >= 0) {
				std::cout << engine.export_parameters().transpose() << std::endl;
			}
		}
	}

	return;
}