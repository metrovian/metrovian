#include "command/operation/abstract.h"
#include "core/operation/correlation.h"

void command_correlation::setup(CLI::App *parent) {
	auto command = parent->add_subcommand("correlation")->group("OPERATION");
	command->add_option("-i, --in", in_, "measured wav")->required();
	command->add_option("-o, --out", out_, "return csv");
	command->callback([this]() { run(); });
	return;
}

void command_correlation::run() {
	operation_correlation engine;
	Eigen::VectorXd sample;
	Eigen::VectorXd acorr;
	Eigen::VectorXd time;
	Eigen::VectorXd waveform;
	Eigen::Index start;
	Eigen::Index len;
	int sample_rate = read_audio(in_, sample);
	if (sample_rate > 0) {
		acorr = engine.operate(sample);
		time = Eigen::VectorXd::LinSpaced(acorr.size(), 0.000E+0, (acorr.size() - 1));
		time /= static_cast<double>(sample_rate);
		for (Eigen::Index i = 1; i < acorr.size() - 1; ++i) {
			if (acorr[i] > acorr[i - 1] &&
			    acorr[i] > acorr[i + 1] &&
			    acorr[i] > 0) {
				sample.maxCoeff(&start);
				len = std::min(Eigen::Index(sample_rate * time[i]), sample.size() - start);
				time = time.head(len);
				waveform = sample.segment(start, len);
				write_vector(out_, time, waveform, ',');
				break;
			}
		}
	}

	return;
}