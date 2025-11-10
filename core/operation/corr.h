#pragma once
#include <Eigen/Dense>

class operation_xcorr {
public: /* overload */
	Eigen::VectorXd operate(const Eigen::VectorXd &sample);
	Eigen::VectorXd operate(const Eigen::VectorXd &sample1, const Eigen::VectorXd &sample2);

protected: /* operation */
	virtual Eigen::VectorXd operation(const Eigen::VectorXd &sample1, const Eigen::VectorXd &sample2);
};