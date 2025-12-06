#include "daemon/server-micro/context.h"

context_singleton &context_singleton::instance() {
	static context_singleton instance_;
	return instance_;
}

void context_singleton::transition(machine::state state) {
	state_ = state;
	return;
}

void context_singleton::transition(machine::waveform waveform) {
	waveform_ = waveform;
	return;
}

machine::state context_singleton::update_state() {
	return state_;
}

machine::waveform context_singleton::update_waveform() {
	return waveform_;
}
