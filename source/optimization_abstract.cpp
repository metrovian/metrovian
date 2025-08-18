#include "optimization_abstract.h"
#include "predefined.h"

Eigen::VectorXd optimization_abstract::export_parameters() {
	return params_;
}

Eigen::VectorXd optimization_abstract::import_parameters(const Eigen::VectorXd &params) {
	params_ = params;
	return export_parameters();
}

double optimization_abstract::residual(const double domain, const double range) {
	mu::Parser parser;
	parser.DefineVar("x", const_cast<double *>(&domain));
	for (size_t i = 0; i < params_.size(); ++i) {
		parser.DefineVar("c" + std::to_string(i + 1), &params_[i]);
	}

	parser.SetExpr(objective_function());
	return range - parser.Eval();
}

double optimization_abstract::residual(const double domain, const double range, const Eigen::VectorXd &params) {
	mu::Parser parser;
	parser.DefineVar("x", const_cast<double *>(&domain));
	for (size_t i = 0; i < params.size(); ++i) {
		parser.DefineVar("c" + std::to_string(i + 1), const_cast<double *>(&params[i]));
	}

	parser.SetExpr(objective_function());
	return range - parser.Eval();
}

Eigen::VectorXd optimization_abstract::residual(const Eigen::VectorXd &domain, const Eigen::VectorXd &range) {
	Eigen::VectorXd residuals(domain.size());
	for (size_t i = 0; i < domain.size(); ++i) {
		residuals[i] = residual(domain[i], range[i]);
	}

	return residuals;
}

Eigen::VectorXd optimization_abstract::residual(const Eigen::VectorXd &domain, const Eigen::VectorXd &range, const Eigen::VectorXd &params) {
	Eigen::VectorXd residuals(domain.size());
	for (size_t i = 0; i < domain.size(); ++i) {
		residuals[i] = residual(domain[i], range[i], params);
	}

	return residuals;
}

Eigen::VectorXd optimization_abstract::derivative(const Eigen::VectorXd &domain, const Eigen::VectorXd &range, const size_t feature) {
	Eigen::VectorXd forward = params_;
	Eigen::VectorXd backward = params_;
	forward[feature] += step_derivative();
	backward[feature] -= step_derivative();
	return (residual(domain, range, forward) - residual(domain, range, backward)) / (step_derivative() * 2.0);
}

Eigen::MatrixXd optimization_abstract::jacobian(const Eigen::VectorXd &domain, const Eigen::VectorXd &range) {
	Eigen::MatrixXd jacobi(params_.size(), domain.size());
	for (size_t i = 0; i < params_.size(); ++i) {
		jacobi.col(i) = derivative(domain, range, i);
	}

	return jacobi.transpose();
}

Eigen::VectorXd optimization_abstract::calibrate(const Eigen::VectorXd &domain, const Eigen::VectorXd &range, const size_t iteration, const double epsilon) {
	LOG_ENTER();
	double damp = step_damp();
	for (size_t i = 0; i < iteration; ++i) {
		Eigen::VectorXd prev = residual(domain, range);
		Eigen::VectorXd step = step_iteration(domain, range, damp);
		params_ -= step;
		residual(domain, range).norm() < prev.norm() ? damp *= step_decrease() : damp *= step_increase();
		if (step.norm() < epsilon) {
			break;
		}
	}

	spdlog::debug("residual: {}", residual(domain, range).norm());
	LOG_EXIT();
	return residual(domain, range);
}

Eigen::VectorXd optimization_abstract::estimate(const Eigen::VectorXd &domain) {
	LOG_ENTER();
	Eigen::VectorXd estimation(domain.size());
	for (size_t i = 0; i < domain.size(); ++i) {
		mu::Parser parser;
		parser.DefineVar("x", const_cast<double *>(&domain[i]));
		for (size_t j = 0; j < params_.size(); ++j) {
			parser.DefineVar("c" + std::to_string(j + 1), &params_[j]);
		}

		parser.SetExpr(objective_function());
		estimation[i] = parser.Eval();
	}

	LOG_EXIT();
	return estimation;
}

Eigen::VectorXd optimization_abstract::step_iteration(const Eigen::VectorXd &domain, const Eigen::VectorXd &range, const double damp) {
	Eigen::VectorXd residuals = residual(domain, range);
	Eigen::MatrixXd jacobi = jacobian(domain, range);
	Eigen::MatrixXd hess = jacobi.transpose() * jacobi;
	Eigen::VectorXd gradient = jacobi.transpose() * residuals;
	Eigen::MatrixXd approximation = hess;
	approximation.diagonal() += damp * hess.diagonal();
	return approximation.ldlt().solve(gradient);
}

std::string optimization_general::export_function() {
	return function_;
}

std::string optimization_general::import_function(const std::string &function) {
	function_ = function;
	return export_function();
}

std::string optimization_general::objective_function() {
	return export_function();
}