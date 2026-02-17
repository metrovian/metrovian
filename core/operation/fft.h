#pragma once
#include <Eigen/Dense>

class operation_fft {
public:
	int operate(const Eigen::VectorXcd &sample, Eigen::VectorXcd &spectrum);
	int operate(const Eigen::VectorXd &sample, Eigen::VectorXcd &spectrum);
	int operate(const Eigen::VectorXd &sample, Eigen::VectorXd &spectrum);

protected:
	Eigen::VectorXcd fft(const Eigen::VectorXcd &sample);
	virtual int operation(const Eigen::VectorXcd &sample, Eigen::VectorXcd &spectrum);
};

class operation_ifft final : public operation_fft {
protected:
	Eigen::VectorXcd ifft(const Eigen::VectorXcd &spectrum);
	virtual int operation(const Eigen::VectorXcd &spectrum, Eigen::VectorXcd &sample) override;
};