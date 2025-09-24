#include "operation/desa.h"

Eigen::VectorXd operation_desa::tkeo(const Eigen::VectorXd &range) {
	Eigen::VectorXd psi = range;
	for (size_t i = 0; i < range.size(); ++i) {
		psi[i] *= psi[i];
		if (i == psi.size() - 1) {
			psi[i] -= range[0] * range[i + 1];
		} else if (i == 0) {
			psi[i] -= range[i - 1] * range[psi.size() - 1];
		} else {
			psi[i] -= range[i - 1] * range[i + 1];
		}
	}

	return psi;
}

int8_t operation_desa::operate(const Eigen::VectorXd &domain, const Eigen::VectorXd &range, Eigen::VectorXd &amplitude, Eigen::VectorXd &frequency) {
	return operation(domain, range, amplitude, frequency);
}

int8_t operation_desa::operate(const Eigen::VectorXd &domain, const Eigen::VectorXd &range, Eigen::VectorXd &amplitude) {
	Eigen::VectorXd frequency;
	return operation(domain, range, amplitude, frequency);
}

int8_t operation_desa1::operation(const Eigen::VectorXd &domain, const Eigen::VectorXd &range, Eigen::VectorXd &amplitude, Eigen::VectorXd &frequency) {
	Eigen::VectorXd prev = Eigen::VectorXd::Zero(range.size());
	for (size_t i = 1; i < range.size(); ++i) {
		prev[i] = range[i - 1];
	}

	amplitude = Eigen::VectorXd::Zero(range.size());
	frequency = Eigen::VectorXd::Zero(range.size());
	Eigen::VectorXd psi = tkeo(range);
	Eigen::VectorXd dsi = tkeo(range - prev);
	for (size_t i = 1; i < range.size(); ++i) {
		if (domain[i] != domain[i - 1]) {
			amplitude[i] = psi[i] / std::sqrt(dsi[i]);
			frequency[i] = std::sqrt(dsi[i] / psi[i]) / (domain[i] - domain[i - 1]);
		}
	}

	return 0;
}

int8_t operation_desa2::operation(const Eigen::VectorXd &domain, const Eigen::VectorXd &range, Eigen::VectorXd &amplitude, Eigen::VectorXd &frequency) {
	return 0;
}
