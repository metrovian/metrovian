#pragma once
#include <Eigen/Dense>

class operation_fft {
protected: /* fourier operator */
	Eigen::VectorXcd fft(const Eigen::VectorXcd &range);

public: /* overload */
	int8_t operate(const Eigen::VectorXd &domain, const Eigen::VectorXcd &range, Eigen::VectorXd &frequency, Eigen::VectorXcd &spectrum);
	int8_t operate(const Eigen::VectorXd &domain, const Eigen::VectorXd &range, Eigen::VectorXd &frequency, Eigen::VectorXcd &spectrum);
	int8_t operate(const Eigen::VectorXd &domain, const Eigen::VectorXd &range, Eigen::VectorXd &frequency, Eigen::VectorXd &spectrum);

protected: /* operation */
	virtual int8_t operation(
	    const Eigen::VectorXd &domain,
	    const Eigen::VectorXcd &range,
	    Eigen::VectorXd &frequency,
	    Eigen::VectorXcd &spectrum);
};

class operation_ifft : public operation_fft {
protected: /* fourier operator */
	Eigen::VectorXcd ifft(const Eigen::VectorXcd &spectrum);

protected: /* operation */
	virtual int8_t operation(
	    const Eigen::VectorXd &frequency,
	    const Eigen::VectorXcd &spectrum,
	    Eigen::VectorXd &domain,
	    Eigen::VectorXcd &range) override final;
};