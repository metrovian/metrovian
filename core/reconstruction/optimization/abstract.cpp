#include "core/reconstruction/optimization/abstract.h"
#include "core/property.h"
#include "core/predefined.h"

std::string optimization_abstract::export_function() {
	return function_;
}

std::string optimization_abstract::import_function(const std::string &function) {
	std::regex rex(R"(c(\d+))");
	std::smatch match;
	size_t dimension = 0;
	auto begin = std::sregex_iterator(function.begin(), function.end(), rex);
	auto end = std::sregex_iterator();
	for (auto it = begin; it != end; ++it) {
		if (dimension < std::stoull((*it)[1].str()) + 1) {
			dimension = std::stoull((*it)[1].str()) + 1;
		}
	}

	if (dimension > 0) {
		params_ = Eigen::VectorXd::Zero(dimension);
	}

	function_ = function;
	return export_function();
}

Eigen::VectorXd optimization_abstract::export_parameters() {
	return params_;
}

Eigen::VectorXd optimization_abstract::import_parameters(const Eigen::VectorXd &params) {
	params_ = params;
	return export_parameters();
}

double optimization_abstract::residual(const double domain, const double range) {
	mu::Parser parser;
	parser.DefineVar("t", const_cast<double *>(&domain));
	for (Eigen::Index i = 0; i < params_.size(); ++i) {
		parser.DefineVar("c" + std::to_string(i), &params_[i]);
	}

	parser.SetExpr(objective_function());
	return range - parser.Eval();
}

double optimization_abstract::residual(const double domain, const double range, const Eigen::VectorXd &params) {
	mu::Parser parser;
	parser.DefineVar("t", const_cast<double *>(&domain));
	for (Eigen::Index i = 0; i < params.size(); ++i) {
		parser.DefineVar("c" + std::to_string(i), const_cast<double *>(&params[i]));
	}

	parser.SetExpr(objective_function());
	return range - parser.Eval();
}

Eigen::VectorXd optimization_abstract::residual(const Eigen::VectorXd &domain, const Eigen::VectorXd &range) {
	Eigen::VectorXd residuals(domain.size());
	for (Eigen::Index i = 0; i < domain.size(); ++i) {
		residuals[i] = residual(domain[i], range[i]);
	}

	return residuals;
}

Eigen::VectorXd optimization_abstract::residual(const Eigen::VectorXd &domain, const Eigen::VectorXd &range, const Eigen::VectorXd &params) {
	Eigen::VectorXd residuals(domain.size());
	for (Eigen::Index i = 0; i < domain.size(); ++i) {
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
	Eigen::MatrixXd jacobi(domain.size(), params_.size());
	auto trd_func = [&](size_t value) {
		jacobi.col(value) = derivative(domain, range, value);
	};

	size_t trd_max = CONFIG_UINT64("optimization", "thread-max");
	size_t trd_q = params_.size() / trd_max;
	size_t trd_r = params_.size() % trd_max;
	for (size_t i = 0; i < trd_q; ++i) {
		std::vector<std::jthread> trds(trd_max);
		for (size_t j = 0; j < trds.size(); ++j) {
			trds[j] = std::jthread(trd_func, trd_max * i + j);
		}
	}

	std::vector<std::jthread> trds(trd_r);
	for (size_t i = 0; i < trds.size(); ++i) {
		trds[i] = std::jthread(trd_func, trd_max * trd_q + i);
	}

	return jacobi;
}

Eigen::VectorXd optimization_abstract::calibrate(const Eigen::VectorXd &domain, const Eigen::VectorXd &range, const size_t iteration, const double epsilon) {
	LOG_ENTER();
	Eigen::MatrixXd jacobi = jacobian(domain, range);
	Eigen::MatrixXd hess = jacobi.transpose() * jacobi;
	Eigen::VectorXd diag = hess.diagonal();
	double damp = *std::max_element(diag.begin(), diag.end()) * step_damp();
	double predicted = 0;
	double measured = 0;
	for (size_t i = 0; i < iteration; ++i) {
		Eigen::VectorXd step = step_iteration(domain, range, damp);
		Eigen::VectorXd prev = residual(domain, range);
		Eigen::VectorXd now = residual(domain, range, params_ - step);
		predicted = prev.norm() - (prev - step).norm();
		measured = prev.norm() - now.norm();
		if (measured > 0.000E+0) {
			if (measured > predicted * 0.750E+0) {
				damp /= step_decrease();
			}

			params_ -= step;
			spdlog::debug("residual-{}: {}", i, now.norm() / std::sqrt(now.size()));
		} else {
			damp *= step_increase();
		}

		if (step.norm() < epsilon) {
			break;
		}
	}

	LOG_EXIT();
	return residual(domain, range);
}

Eigen::VectorXd optimization_abstract::estimate(const Eigen::VectorXd &domain) {
	LOG_ENTER();
	Eigen::VectorXd estimation(domain.size());
	for (Eigen::Index i = 0; i < domain.size(); ++i) {
		mu::Parser parser;
		parser.DefineVar("x", const_cast<double *>(&domain[i]));
		for (Eigen::Index j = 0; j < params_.size(); ++j) {
			parser.DefineVar("c" + std::to_string(j + 1), &params_[j]);
		}

		parser.SetExpr(objective_function());
		estimation[i] = parser.Eval();
	}

	LOG_EXIT();
	return estimation;
}

double optimization_abstract::step_derivative() {
	static const double step = CONFIG_FLOAT64("optimization", "derivative-step");
	return step;
}

double optimization_abstract::step_damp() {
	static const double step = CONFIG_FLOAT64("optimization", "damp-step");
	return step;
}

double optimization_abstract::step_increase() {
	static const double step = CONFIG_FLOAT64("optimization", "increase-step");
	return step;
}

double optimization_abstract::step_decrease() {
	static const double step = CONFIG_FLOAT64("optimization", "decrease-step");
	return step;
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