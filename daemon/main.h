#pragma once
#include "core/synthesis/abstract.h"
#include "state/abstract.h"
#include "state/setup.h"
#include "state/synthesis.h"
#include "state/performance.h"

class machine_singleton {
protected: /* machine core */
	std::unique_ptr<synthesis_abstract> core_ = nullptr;

protected: /* machine state */
	std::atomic<machine::state> state_ = machine::state::startup;
	std::unordered_map<machine::state, std::unique_ptr<state_abstract>> map_;

protected: /* friend */
	friend class state_abstract;
	friend class state_setup;
	friend class state_synthesis;
	friend class state_performance;

protected: /* friend */
	void setup(std::unique_ptr<synthesis_abstract> core);
	void synthesize();
	void perform();
	void terminate();
	void transition(machine::state next);

public: /* export */
	void loop();

public: /* instance */
	static machine_singleton &instance();

private: /* load */
	void load_map();
	void load_stdout();
	void load_stderr();

private: /* constraint */
	machine_singleton();
	machine_singleton(const machine_singleton &) = default;
	machine_singleton &operator=(const machine_singleton &) = default;
};