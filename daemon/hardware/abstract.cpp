#include "daemon/hardware/abstract.h"

void hardware_abstract::enter(machine::state state) {
	// clang-format off
	switch (state) {
	case machine::state::setup:
		knob_->start();
		indicator_->set(indicator::state::blink_slow);
		break;
	case machine::state::synthesis: indicator_->set(indicator::state::blink_fast); break;
	case machine::state::performance: indicator_->set(indicator::state::on); break;
	default: indicator_->set(indicator::state::off); break;
	}
	// clang-format on
	return;
}

void hardware_abstract::exit(machine::state state) {
	if (state == machine::state::setup) {
		knob_->stop();
	}

	return;
}

void hardware_abstract::create() {
	indicator_ = create_indicator();
	knob_ = create_knob();
	return;
}