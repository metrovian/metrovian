#include "operation/fft.h"

Eigen::VectorXcd operation_fft::fft(const Eigen::VectorXcd &range) {
	if (range.size() == 1) {
		return range;
	}

	Eigen::VectorXcd result = Eigen::VectorXcd::Zero(range.size());
	Eigen::VectorXcd even = range(Eigen::seq(0, range.size() - 1, 2));
	Eigen::VectorXcd odd = range(Eigen::seq(1, range.size() - 1, 2));
	Eigen::VectorXcd feven = fft(even);
	Eigen::VectorXcd fodd = fft(odd);
	for (Eigen::Index i = 0; i < range.size() / 2; ++i) {
		Eigen::dcomplex twiddle = std::polar(1.000E+0, -2.000E+0 * (M_PI * i) / range.size());
		result[i] = feven[i] + fodd[i] * twiddle;
		result[i + range.size() / 2] = feven[i] - fodd[i] * twiddle;
	}

	return result;
}

int8_t operation_fft::operate(const Eigen::VectorXd &domain, const Eigen::VectorXcd &range, Eigen::VectorXd &frequency, Eigen::VectorXcd &spectrum) {
	return operation(domain, range, frequency, spectrum);
}

int8_t operation_fft::operate(const Eigen::VectorXd &domain, const Eigen::VectorXd &range, Eigen::VectorXd &frequency, Eigen::VectorXcd &spectrum) {
	Eigen::VectorXcd range_complex = Eigen::VectorXcd::Zero(range.size());
	range_complex.real() = range;
	return operation(domain, range_complex, frequency, spectrum);
}

int8_t operation_fft::operate(const Eigen::VectorXd &domain, const Eigen::VectorXd &range, Eigen::VectorXd &frequency, Eigen::VectorXd &spectrum) {
	Eigen::VectorXcd range_complex = Eigen::VectorXcd::Zero(range.size());
	Eigen::VectorXcd spectrum_complex = Eigen::VectorXcd::Zero(spectrum.size());
	range_complex.real() = range;
	int8_t retcode = operation(domain, range_complex, frequency, spectrum_complex);
	spectrum = spectrum_complex.array().abs();
	return retcode;
}

int8_t operation_fft::operation(const Eigen::VectorXd &domain, const Eigen::VectorXcd &range, Eigen::VectorXd &frequency, Eigen::VectorXcd &spectrum) {
	if (range.size() < 2) {
		return -1;
	}

	Eigen::Index len_pad = 1;
	while (len_pad < range.size()) {
		len_pad <<= 1;
	}

	Eigen::VectorXcd pad = Eigen::VectorXcd::Zero(len_pad);
	for (Eigen::Index i = 0; i < range.size(); ++i) {
		pad[i] = range[i];
	}

	Eigen::VectorXd bin = Eigen::VectorXd::Zero(len_pad);
	for (Eigen::Index i = 0; i < bin.size(); ++i) {
		bin[i] = static_cast<double>(i) / (domain[1] - domain[0]);
	}

	Eigen::VectorXcd pow = fft(pad);
	frequency = bin / len_pad;
	spectrum = pow / len_pad;
	return 0;
}

Eigen::VectorXcd operation_ifft::ifft(const Eigen::VectorXcd &spectrum) {
	if (spectrum.size() == 1) {
		return spectrum;
	}

	Eigen::VectorXcd result = fft(spectrum.conjugate()).conjugate() / spectrum.size();
	return result;
}

int8_t operation_ifft::operation(const Eigen::VectorXd &frequency, const Eigen::VectorXcd &spectrum, Eigen::VectorXd &domain, Eigen::VectorXcd &range) {
	if (spectrum.size() < 2) {
		return -1;
	}

	Eigen::Index len_pad = 1;
	while (len_pad < spectrum.size()) {
		len_pad <<= 1;
	}

	Eigen::VectorXcd pad = Eigen::VectorXcd::Zero(len_pad);
	for (Eigen::Index i = 0; i < spectrum.size(); ++i) {
		pad[i] = spectrum[i];
	}

	Eigen::VectorXd bin = Eigen::VectorXd::Zero(len_pad);
	for (Eigen::Index i = 0; i < bin.size(); ++i) {
		bin[i] = static_cast<double>(i) / (frequency[1] - frequency[0]);
	}

	Eigen::VectorXcd pow = ifft(pad);
	domain = bin / len_pad;
	range = pow * len_pad;
	return 0;
}