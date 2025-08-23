#include "optimization_user.h"
#include "predefined.h"

std::string optimization_user::export_function() {
	return function_;
}

std::string optimization_user::import_function(const std::string &function) {
	std::regex rex(R"(c(\d+))");
	std::smatch match;
	size_t dimension = 0;
	auto begin = std::sregex_iterator(function.begin(), function.end(), rex);
	auto end = std::sregex_iterator();
	for (auto it = begin; it != end; ++it) {
		if (dimension < std::stoi((*it)[1].str())) {
			dimension = std::stoi((*it)[1].str());
		}
	}

	if (dimension > 0) {
		params_ = Eigen::VectorXd::Zero(dimension);
	}

	function_ = function;
	return export_function();
}

std::string optimization_user::objective_function() {
	return export_function();
}