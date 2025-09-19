#include "reconstruction/optimization/polynomial.h"
#include "predefined.h"

std::string optimization_polynomial::import_function(uint8_t model) {
	std::ostringstream oss;
	oss << "c0";
	for (uint8_t i = 0; i < model; ++i) {
		oss << " + ";
		oss << "c" << i + 1 << " * x";
		if (i > 0) {
			oss << "^" << i + 1;
		}
	}

	return optimization_abstract::import_function(oss.str());
}

std::string optimization_polynomial::objective_function() {
	return export_function();
}