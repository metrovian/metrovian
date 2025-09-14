#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <algorithm>
#include <regex>
#include <sstream>
#include <string>
#include <stdint.h>
#include <spdlog/spdlog.h>
#include <muParser.h>
#include <Eigen/Dense>

class optimization_abstract {
protected: /* function */
	std::string function_;

public: /* function */
	std::string export_function();
	std::string import_function(const std::string &function);

protected: /* paramter */
	Eigen::VectorXd params_;

public: /* parameter */
	Eigen::VectorXd export_parameters();
	Eigen::VectorXd import_parameters(const Eigen::VectorXd &params);

protected: /* residual */
	double residual(const double domain, const double range);
	double residual(const double domain, const double range, const Eigen::VectorXd &params);
	Eigen::VectorXd residual(const Eigen::VectorXd &domain, const Eigen::VectorXd &range);
	Eigen::VectorXd residual(const Eigen::VectorXd &domain, const Eigen::VectorXd &range, const Eigen::VectorXd &params);
	Eigen::VectorXd derivative(const Eigen::VectorXd &domain, const Eigen::VectorXd &range, const size_t feature);
	Eigen::MatrixXd jacobian(const Eigen::VectorXd &domain, const Eigen::VectorXd &range);

public: /* calibration */
	Eigen::VectorXd calibrate(const Eigen::VectorXd &domain, const Eigen::VectorXd &range, const size_t iteration, const double epsilon);
	Eigen::VectorXd estimate(const Eigen::VectorXd &domain);

public: /* abstract */
	~optimization_abstract() {}

protected: /* abstract */
	virtual double step_derivative() { return 1.000E-5; }
	virtual double step_damp() { return 1.000E-3; }
	virtual double step_increase() { return 1.000E+1; }
	virtual double step_decrease() { return 1.000E+1; }
	virtual Eigen::VectorXd step_iteration(const Eigen::VectorXd &domain, const Eigen::VectorXd &range, const double damp);
	virtual std::string objective_function() = 0;
};