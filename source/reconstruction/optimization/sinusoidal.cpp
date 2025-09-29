#include "reconstruction/optimization/sinusoidal.h"
#include "predefined.h"

std::string optimization_sinusoidal::import_function(sinusoidal::function model) {
	// clang-format off
	std::string function;
	switch (model) {
	case sinusoidal::function::un_damped: function = std::string("c0 + c1 * cos(c2 * t + c3)"); break;
	case sinusoidal::function::under_damped: function = std::string("c0 + c1 * exp(c2 * t) * cos(c3 * t + c4)"); break;
	case sinusoidal::function::over_damped: function = std::string("c0 + c1 * exp(c2 * t) + c3 * exp(c4 * t)"); break;
	case sinusoidal::function::critical_damped: function = std::string("c0 + (c1 + c2 * t) * exp(c3 * t)"); break;
	}
	// clang-format on
	return optimization_abstract::import_function(function);
}

std::string optimization_sinusoidal::objective_function() {
	return export_function();
}