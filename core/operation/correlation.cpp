#include "core/operation/correlation.h"
#include "core/operation/fourier.h"

Eigen::VectorXd operation_correlation::operate(const Eigen::VectorXd &sample) {
	return operation(sample, sample);
}

Eigen::VectorXd operation_correlation::operate(const Eigen::VectorXd &sample1, const Eigen::VectorXd &sample2) {
	return operation(sample1, sample2);
}

Eigen::VectorXd operation_correlation::operation(const Eigen::VectorXd &sample1, const Eigen::VectorXd &sample2) {
	operation_fft fft;
	operation_ifft ifft;
	Eigen::VectorXcd spectrum1;
	Eigen::VectorXcd spectrum2;
	Eigen::VectorXcd result;
	fft.operate(sample1, spectrum1);
	fft.operate(sample2, spectrum2);
	ifft.operate(spectrum1.conjugate().cwiseProduct(spectrum2).eval(), result);
	return result.head(sample1.size()).real();
}