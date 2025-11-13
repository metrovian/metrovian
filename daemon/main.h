#pragma once
#include "state/abstract.h"
#include "state/wait.h"
#include "state/synthesis.h"
#include "state/performance.h"

class machine_singleton {
protected: /* machine state */
	std::atomic<machine::state> state_ = machine::state::wait;
	std::unordered_map<machine::state, std::unique_ptr<state_abstract>> map_;

public: /* export */
	int8_t run();
	int8_t terminate();
	int8_t transition(machine::state next);

public: /* instance */
	static machine_singleton &instance();

private: /* constraint */
	machine_singleton();
	machine_singleton(const machine_singleton &) = default;
	machine_singleton &operator=(const machine_singleton &) = default;
};