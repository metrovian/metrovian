#include "core/operation/desa.h"

int operation_desa::operate(const Eigen::VectorXd &sample, Eigen::VectorXd &iamp, Eigen::VectorXd &irate) {
	return operation(sample, iamp, irate);
}

int operation_desa::operate(const Eigen::VectorXd &sample, Eigen::VectorXd &iamp) {
	Eigen::VectorXd irate;
	return operation(sample, iamp, irate);
}

Eigen::VectorXd operation_desa::tkeo(const Eigen::VectorXd &sample) {
	Eigen::VectorXd psi = Eigen::VectorXd::Zero(sample.size());
	for (Eigen::Index i = 1; i < sample.size() - 1; ++i) {
		psi[i] += sample[i] * sample[i];
		psi[i] -= sample[i - 1] * sample[i + 1];
	}

	return psi;
}

int operation_desa1::operation(const Eigen::VectorXd &sample, Eigen::VectorXd &iamp, Eigen::VectorXd &irate) {
	Eigen::VectorXd prev = Eigen::VectorXd::Zero(sample.size());
	for (Eigen::Index i = 1; i < sample.size(); ++i) {
		prev[i] = sample[i - 1];
	}

	iamp = Eigen::VectorXd::Zero(sample.size());
	irate = Eigen::VectorXd::Zero(sample.size());
	Eigen::VectorXd psi = tkeo(sample);
	Eigen::VectorXd dsi = tkeo((sample - prev) / 1.000E+0);
	for (Eigen::Index i = 1; i < sample.size() - 1; ++i) {
		iamp[i] = psi[i] / std::sqrt(dsi[i]);
		irate[i] = std::sqrt(dsi[i] / psi[i]);
	}

	return 0;
}

int operation_desa2::operation(const Eigen::VectorXd &sample, Eigen::VectorXd &iamp, Eigen::VectorXd &irate) {
	Eigen::VectorXd prev = Eigen::VectorXd::Zero(sample.size());
	Eigen::VectorXd next = Eigen::VectorXd::Zero(sample.size());
	for (Eigen::Index i = 1; i < sample.size(); ++i) {
		prev[i] = sample[i - 1];
		next[i - 1] = sample[i];
	}

	iamp = Eigen::VectorXd::Zero(sample.size());
	irate = Eigen::VectorXd::Zero(sample.size());
	Eigen::VectorXd psi = tkeo(sample);
	Eigen::VectorXd dsi = tkeo((next - prev) / 2.000E+0);
	for (Eigen::Index i = 1; i < sample.size() - 1; ++i) {
		iamp[i] = psi[i] / std::sqrt(dsi[i]);
		irate[i] = std::sqrt(dsi[i] / psi[i]);
	}

	return 0;
}
