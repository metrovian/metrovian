#include "core/synthesis/add.h"
#include "core/property.h"
#include "core/predefined.h"

synthesis_add::synthesis_add(const nlohmann::ordered_json &preset) {
	if (preset.value("method", "") != std::string("add")) {
		return;
	}

	if (preset["components"].is_array() == true) {
		for (const auto &object : preset["components"]) {
			auto iter = synthesis_oscillator::map().find(object.value("osc", ""));
			if (iter != synthesis_oscillator::map().end()) {
				components_.push_back(iter->second(object));
				amps_.push_back(object.value("amp", 0.0));
				ratios_.push_back(object.value("ratio", 1.0));
			}
		}
	}
}

void synthesis_add::synthesis(uint64_t min, uint64_t max, uint64_t period) {
	LOG_ENTER();
	double sum = 0.000E+0;
	double peak = 0.000E+0;
	for (uint64_t i = 0; i < components_.size(); ++i) {
		peak += amps_[i];
	}

	uint64_t sample_rate = CONFIG_UINT64("synthesis", "sample-rate");
	std::vector<int16_t> sample(sample_rate * period, 0);
	set_size(max);
	for (uint64_t i = min; i < max; ++i) {
		if (on_synthesis_ != nullptr) {
			on_synthesis_((i + 1) - min);
		}

		for (uint64_t j = 0; j < sample.size(); ++j) {
			sum = 0.000E+0;
			for (uint64_t k = 0; k < components_.size(); ++k) {
				sum += components_[k](i + std::log2(ratios_[k]) * 1.200E+1, j) *
				       amps_[k] / peak;
			}

			sample[j] = static_cast<int16_t>(sum * 3276);
		}

		set_sample(i, sample);
		LOG_PROGRESS((i + 1) - min, max - min);
	}

	LOG_EXIT();
	return;
}
