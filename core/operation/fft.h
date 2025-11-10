#pragma once
#include <Eigen/Dense>

class operation_fft {
protected: /* fourier operator */
	Eigen::VectorXcd fft(const Eigen::VectorXcd &sample);

public: /* overload */
	int8_t operate(const Eigen::VectorXcd &sample, Eigen::VectorXcd &spectrum);
	int8_t operate(const Eigen::VectorXd &sample, Eigen::VectorXcd &spectrum);
	int8_t operate(const Eigen::VectorXd &sample, Eigen::VectorXd &spectrum);

protected: /* operation */
	virtual int8_t operation(const Eigen::VectorXcd &sample, Eigen::VectorXcd &spectrum);
};

class operation_ifft : public operation_fft {
protected: /* fourier operator */
	Eigen::VectorXcd ifft(const Eigen::VectorXcd &spectrum);

protected: /* operation */
	virtual int8_t operation(const Eigen::VectorXcd &spectrum, Eigen::VectorXcd &sample) override final;
};