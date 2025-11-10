#pragma once
#include <Eigen/Dense>

class operation_desa {
protected: /* energy operator */
	Eigen::VectorXd tkeo(const Eigen::VectorXd &sample);

public: /* overload */
	int8_t operate(const Eigen::VectorXd &sample, Eigen::VectorXd &iamp, Eigen::VectorXd &irate);
	int8_t operate(const Eigen::VectorXd &sample, Eigen::VectorXd &iamp);

protected: /* operation */
	virtual int8_t operation(
	    const Eigen::VectorXd &sample,
	    Eigen::VectorXd &iamp,
	    Eigen::VectorXd &irate) = 0;
};

class operation_desa1 : public operation_desa {
protected: /* operation */
	virtual int8_t operation(
	    const Eigen::VectorXd &sample,
	    Eigen::VectorXd &iamp,
	    Eigen::VectorXd &irate) override final;
};

class operation_desa2 : public operation_desa {
protected: /* operation */
	virtual int8_t operation(
	    const Eigen::VectorXd &sample,
	    Eigen::VectorXd &iamp,
	    Eigen::VectorXd &irate) override final;
};