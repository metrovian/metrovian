#include "core/operation/fourier.h"

int operation_fft::operate(const Eigen::VectorXcd &sample, Eigen::VectorXcd &spectrum) {
	return operation(sample, spectrum);
}

int operation_fft::operate(const Eigen::VectorXd &sample, Eigen::VectorXcd &spectrum) {
	Eigen::VectorXcd sample_complex = Eigen::VectorXcd::Zero(sample.size());
	sample_complex.real() = sample;
	return operation(sample_complex, spectrum);
}

int operation_fft::operate(const Eigen::VectorXd &sample, Eigen::VectorXd &spectrum) {
	Eigen::VectorXcd sample_complex = Eigen::VectorXcd::Zero(sample.size());
	Eigen::VectorXcd spectrum_complex = Eigen::VectorXcd::Zero(spectrum.size());
	sample_complex.real() = sample;
	int retcode = operation(sample_complex, spectrum_complex);
	spectrum = spectrum_complex.array().abs();
	return retcode;
}

Eigen::VectorXcd operation_fft::fft(const Eigen::VectorXcd &sample) {
	if (sample.size() == 1) {
		return sample;
	}

	Eigen::VectorXcd result = Eigen::VectorXcd::Zero(sample.size());
	Eigen::VectorXcd even = sample(Eigen::seq(0, sample.size() - 1, 2));
	Eigen::VectorXcd odd = sample(Eigen::seq(1, sample.size() - 1, 2));
	Eigen::VectorXcd feven = fft(even);
	Eigen::VectorXcd fodd = fft(odd);
	for (Eigen::Index i = 0; i < sample.size() / 2; ++i) {
		Eigen::dcomplex twiddle = std::polar(1.000E+0, -2.000E+0 * (M_PI * i) / sample.size());
		result[i] = feven[i] + fodd[i] * twiddle;
		result[i + sample.size() / 2] = feven[i] - fodd[i] * twiddle;
	}

	return result;
}

int operation_fft::operation(const Eigen::VectorXcd &sample, Eigen::VectorXcd &spectrum) {
	if (sample.size() < 2) {
		return -1;
	}

	Eigen::Index len_pad = 1;
	while (len_pad < sample.size()) {
		len_pad <<= 1;
	}

	Eigen::VectorXcd pad = Eigen::VectorXcd::Zero(len_pad);
	for (Eigen::Index i = 0; i < sample.size(); ++i) {
		pad[i] = sample[i];
	}

	spectrum = fft(pad) / len_pad;
	return 0;
}

Eigen::VectorXcd operation_ifft::ifft(const Eigen::VectorXcd &spectrum) {
	if (spectrum.size() == 1) {
		return spectrum;
	}

	Eigen::VectorXcd result = fft(spectrum.conjugate()).conjugate() / spectrum.size();
	return result;
}

int operation_ifft::operation(const Eigen::VectorXcd &spectrum, Eigen::VectorXcd &sample) {
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

	sample = ifft(pad) * len_pad;
	return 0;
}