#include "reconstruction/optimization/transcendental.h"
#include "predefined.h"

std::string optimization_transcendental::import_function(transcendental::function model) {
	// clang-format off
	std::string function;
	switch (model) {
	case transcendental::function::exponential: function = std::string("c0 + c1 * exp(c2 * x)"); break;
	case transcendental::function::ln: function = std::string("c0 + c1 * ln(x)"); break;
	}
	// clang-format on
	return optimization_abstract::import_function(function);
}

std::string optimization_transcendental::objective_function() {
	return export_function();
}