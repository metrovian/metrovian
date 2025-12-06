#pragma once
#include "daemon/state/abstract.h"

class context_singleton {
protected: /* context */
	machine::state state_ = machine::state::setup;
	machine::waveform waveform_ = machine::waveform::none;

public: /* instance */
	static context_singleton &instance();

public: /* setter */
	void transition(machine::state state);
	void transition(machine::waveform waveform);

public: /* getter */
	machine::state update_state();
	machine::waveform update_waveform();

private: /* constraint */
	context_singleton() {}
	context_singleton(const context_singleton &) = default;
	context_singleton &operator=(const context_singleton &) = default;
};