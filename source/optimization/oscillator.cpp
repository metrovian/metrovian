#include "optimization/oscillator.h"
#include "predefined.h"

std::string optimization_oscillator::import_function(oscillator::motion model) {
	// clang-format off
	std::string function;
	switch (model) {
	case oscillator::motion::under_damped: function = std::string("c0 + c1 * exp(c2 * x) * cos(c3 * x + c4)"); break;
	case oscillator::motion::over_damped: function = std::string("c0 + c1 * exp(c2 * x) + c3 * exp(c4 * x)"); break;
	case oscillator::motion::critical_damped: function = std::string("c0 + (c1 + c2 * x) * exp(c3 * x)"); break;
	}
	// clang-format on
	return optimization_abstract::import_function(function);
}

std::string optimization_oscillator::objective_function() {
	return export_function();
}