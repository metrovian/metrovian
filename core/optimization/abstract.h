#pragma once
#include <string>
#include <Eigen/Dense>

class optimization_abstract {
public:
	virtual ~optimization_abstract() = default;
	Eigen::VectorXd calibrate(
	    const Eigen::VectorXd &domain,
	    const Eigen::VectorXd &range,
	    const size_t iteration,
	    const double epsilon);

	Eigen::VectorXd estimate(const Eigen::VectorXd &domain);
	Eigen::VectorXd export_parameters();
	Eigen::VectorXd import_parameters(const Eigen::VectorXd &params);
	std::string export_function();
	std::string import_function(const std::string &func);

protected:
	virtual std::string objective_function() = 0;

private:
	double residual(const double domain, const double range);
	double residual(const double domain, const double range, const Eigen::VectorXd &params);
	Eigen::VectorXd residual(const Eigen::VectorXd &domain, const Eigen::VectorXd &range);
	Eigen::VectorXd residual(
	    const Eigen::VectorXd &domain,
	    const Eigen::VectorXd &range,
	    const Eigen::VectorXd &params);

	Eigen::VectorXd derivative(
	    const Eigen::VectorXd &domain,
	    const Eigen::VectorXd &range,
	    const size_t feature);

	Eigen::MatrixXd jacobian(
	    const Eigen::VectorXd &domain,
	    const Eigen::VectorXd &range);

	double step_derivative();
	double step_damp();
	double step_increase();
	double step_decrease();
	Eigen::VectorXd step_iteration(
	    const Eigen::VectorXd &domain,
	    const Eigen::VectorXd &range,
	    const double damp);

private:
	Eigen::VectorXd params_;
	std::string func_;
};