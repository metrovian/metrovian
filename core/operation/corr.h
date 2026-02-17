#pragma once
#include <Eigen/Dense>

class operation_xcorr {
public:
	Eigen::VectorXd operate(const Eigen::VectorXd &sample);
	Eigen::VectorXd operate(const Eigen::VectorXd &sample1, const Eigen::VectorXd &sample2);

private:
	Eigen::VectorXd operation(const Eigen::VectorXd &sample1, const Eigen::VectorXd &sample2);
};