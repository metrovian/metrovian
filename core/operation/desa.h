#pragma once
#include <Eigen/Dense>

class operation_desa {
public:
	int operate(const Eigen::VectorXd &sample, Eigen::VectorXd &iamp, Eigen::VectorXd &irate);
	int operate(const Eigen::VectorXd &sample, Eigen::VectorXd &iamp);

protected:
	Eigen::VectorXd tkeo(const Eigen::VectorXd &sample);
	virtual int operation(
	    const Eigen::VectorXd &sample,
	    Eigen::VectorXd &iamp,
	    Eigen::VectorXd &irate) = 0;
};

class operation_desa1 final : public operation_desa {
protected:
	int operation(
	    const Eigen::VectorXd &sample,
	    Eigen::VectorXd &iamp,
	    Eigen::VectorXd &irate) override;
};

class operation_desa2 final : public operation_desa {
protected:
	int operation(
	    const Eigen::VectorXd &sample,
	    Eigen::VectorXd &iamp,
	    Eigen::VectorXd &irate) override;
};