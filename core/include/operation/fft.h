#pragma once
#include <Eigen/Dense>

class operation_fft {
protected: /* fourier operator */
	Eigen::VectorXcd fft(const Eigen::VectorXcd &range);

public: /* overload */
	int8_t operate(
	    const Eigen::VectorXd &domain,
	    const Eigen::VectorXd &range,
	    Eigen::VectorXd &frequency,
	    Eigen::VectorXd &spectrum);

protected: /* operation */
	int8_t operation(
	    const Eigen::VectorXd &domain,
	    const Eigen::VectorXd &range,
	    Eigen::VectorXd &frequency,
	    Eigen::VectorXd &spectrum);
};