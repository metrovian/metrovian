#pragma once
#include <Eigen/Dense>

class operation_desa {
protected: /* energy operator */
	Eigen::VectorXd tkeo(const Eigen::VectorXd &range);

public: /* overload */
	int8_t operate(const Eigen::VectorXd &range, Eigen::VectorXd &amplitude, Eigen::VectorXd &frequency);
	int8_t operate(const Eigen::VectorXd &range, Eigen::VectorXd &amplitude);

protected: /* operation */
	virtual int8_t operation(const Eigen::VectorXd &range) = 0;
};

class operation_desa1 : public operation_desa {
protected: /* operation */
	virtual int8_t operation(const Eigen::VectorXd &range);
};

class operation_desa2 : public operation_desa {
protected: /* operation */
	virtual int8_t operation(const Eigen::VectorXd &range);
};