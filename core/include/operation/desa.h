#pragma once
#include <Eigen/Dense>

class operation_desa {
protected: /* energy operator */
	Eigen::VectorXd tkeo(const Eigen::VectorXd &range);

public: /* overload */
	int8_t operate(const Eigen::VectorXd &domain, const Eigen::VectorXd &range, Eigen::VectorXd &amplitude, Eigen::VectorXd &frequency);
	int8_t operate(const Eigen::VectorXd &domain, const Eigen::VectorXd &range, Eigen::VectorXd &amplitude);

protected: /* operation */
	virtual int8_t operation(
	    const Eigen::VectorXd &domain,
	    const Eigen::VectorXd &range,
	    Eigen::VectorXd &amplitude,
	    Eigen::VectorXd &frequency) = 0;
};

class operation_desa1 : public operation_desa {
protected: /* operation */
	virtual int8_t operation(
	    const Eigen::VectorXd &domain,
	    const Eigen::VectorXd &range,
	    Eigen::VectorXd &amplitude,
	    Eigen::VectorXd &frequency) override final;
};

class operation_desa2 : public operation_desa {
protected: /* operation */
	virtual int8_t operation(
	    const Eigen::VectorXd &domain,
	    const Eigen::VectorXd &range,
	    Eigen::VectorXd &amplitude,
	    Eigen::VectorXd &frequency) override final;
};