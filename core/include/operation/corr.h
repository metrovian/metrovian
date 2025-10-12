#pragma once
#include <Eigen/Dense>

class operation_xcorr {
public: /* overload */
	Eigen::VectorXd operate(const Eigen::VectorXd &range1, const Eigen::VectorXd &range2);

protected: /* operation */
	virtual Eigen::VectorXd operation(const Eigen::VectorXd &range1, const Eigen::VectorXd &range2);
};

class operation_acorr : public operation_xcorr {
public: /* overload */
	Eigen::VectorXd operate(const Eigen::VectorXd &range);
};