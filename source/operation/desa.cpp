#include "operation/desa.h"

Eigen::VectorXd operation_desa::tkeo(const Eigen::VectorXd &range) {
	Eigen::VectorXd psi = range;
	for (size_t i = 0; i < psi.size(); ++i) {
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