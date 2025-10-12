#include "operation/corr.h"
#include "operation/fft.h"

Eigen::VectorXd operation_xcorr::operate(const Eigen::VectorXd &range1, const Eigen::VectorXd &range2) {
	return operation(range1, range2);
}

Eigen::VectorXd operation_xcorr::operation(const Eigen::VectorXd &range1, const Eigen::VectorXd &range2) {
	operation_fft fft;
	operation_ifft ifft;
	Eigen::VectorXd domain1 = Eigen::VectorXd::LinSpaced(2, 0.000E+0, 1.000E+0);
	Eigen::VectorXd domain2 = Eigen::VectorXd::LinSpaced(2, 0.000E+0, 1.000E+0);
	Eigen::VectorXd frequency1;
	Eigen::VectorXd frequency2;
	Eigen::VectorXcd spectrum1;
	Eigen::VectorXcd spectrum2;
	Eigen::VectorXcd result;
	fft.operate(domain1, range1, frequency1, spectrum1);
	fft.operate(domain2, range2, frequency2, spectrum2);
	ifft.operate(
	    frequency1,
	    spectrum1.conjugate().cwiseProduct(spectrum2).eval(),
	    domain1,
	    result);

	return result.head(range1.size()).real();
}

Eigen::VectorXd operation_acorr::operate(const Eigen::VectorXd &range) {
	return operation(range, range);
}