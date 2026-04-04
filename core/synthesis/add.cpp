#include "core/synthesis/add.h"
#include "core/synthesis/oscillator.h"
#include "core/property.h"
#include "core/predefined.h"

synthesis_add::synthesis_add(const nlohmann::ordered_json &preset)
    : synthesis_abstract(preset) {
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

void synthesis_add::synthesis(uint64_t max) {
	LOG_ENTER();
	double peak = 0.000E+0;
	for (uint64_t i = 0; i < components_.size(); ++i) {
		peak += amps_[i];
	}

	set_size(max);
	set_synthesis([this, peak](uint64_t note, uint64_t sample) {
		double sum = 0.000E+0;
		for (uint64_t i = 0; i < components_.size(); ++i) {
			sum += components_[i](note + std::log2(ratios_[i]) * 1.200E+1, sample) *
			       amps_[i] / peak;
		}

		return static_cast<int16_t>(sum * 3276);
	});

	LOG_EXIT();
	return;
}
