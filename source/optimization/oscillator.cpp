#include "optimization/oscillator.h"
#include "predefined.h"

std::string optimization_oscillator::import_function(oscillator::motion model) {
	// clang-format off
	std::string function;
	switch (model) {
	case oscillator::motion::harmonic: function = std::string("c1 * exp(c2 * x) * cos(c3 * x + c4) + c5");
	}
	// clang-format on

	return optimization_abstract::import_function(function);
}

std::string optimization_oscillator::objective_function() {
	return export_function();
}