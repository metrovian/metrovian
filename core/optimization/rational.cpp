#include "core/optimization/rational.h"
#include "core/predefined.h"

std::string optimization_rational::import_function(std::vector<uint8_t> model) {
	std::ostringstream oss;
	if (model.size() == 2) {
		oss << "(c0";
		for (uint8_t i = 0; i < model[0]; ++i) {
			oss << " + ";
			oss << "c" << i + 1 << " * t";
			if (i > 0) {
				oss << "^" << i + 1;
			}
		}

		oss << ") / (1";
		for (uint8_t i = 0; i < model[1]; ++i) {
			oss << " + ";
			oss << "c" << static_cast<uint16_t>(model[0]) + static_cast<uint16_t>(i + 1) << " * t";
			if (i > 0) {
				oss << "^" << i + 1;
			}
		}

		oss << ")";
	}

	return optimization_abstract::import_function(oss.str());
}