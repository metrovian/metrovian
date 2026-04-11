#include "core/optimization/polynomial.h"
#include "core/predefined.h"

std::string optimization_polynomial::import_function(uint8_t model) {
	std::ostringstream oss;
	oss << "c0";
	for (uint8_t i = 0; i < model; ++i) {
		oss << " + x*(c" << i + 1;
	}

	for (uint8_t i = 0; i < model; ++i) {
		oss << ")";
	}

	return optimization_abstract::import_function(oss.str());
}