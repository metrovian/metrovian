#include "reconstruction/optimization/transcendental.h"
#include "predefined.h"

std::string optimization_transcendental::import_function(transcendental::function model) {
	// clang-format off
	std::string function;
	switch (model) {
	case transcendental::function::exp: function = std::string("c0 + c1 * exp(c2 * t)"); break;
	case transcendental::function::ln: function = std::string("c0 + c1 * ln(t)"); break;
	case transcendental::function::sinh: function = std::string("c0 + c1 * sinh(c2 * t + c3)"); break;
	case transcendental::function::cosh: function = std::string("c0 + c1 * cosh(c2 * t + c3)"); break;
	case transcendental::function::tanh: function = std::string("c0 + c1 * tanh(c2 * t + c3)"); break;
	}
	// clang-format on
	return optimization_abstract::import_function(function);
}

std::string optimization_transcendental::objective_function() {
	return export_function();
}