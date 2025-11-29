#pragma once
#include "daemon/hardware/abstract.h"
#include "daemon/hardware/development.h"
#include "daemon/hardware/raspi.h"
#include "daemon/state/abstract.h"
#include "daemon/state/dummy.h"
#include "daemon/state/setup.h"
#include "daemon/state/synthesis.h"
#include "daemon/state/performance.h"
#include "core/synthesis/abstract.h"

class machine_singleton {
protected: /* machine core */
	std::unique_ptr<synthesis_abstract> core_ = nullptr;
	std::unique_ptr<hardware_abstract> hw_ = nullptr;

protected: /* machine state */
	std::atomic<machine::state> state_ = machine::state::startup;
	std::unordered_map<machine::state, std::unique_ptr<state_abstract>> map_;

protected: /* friend */
	friend class state_abstract;
	friend class state_setup;
	friend class state_synthesis;
	friend class state_performance;

public: /* instance */
	static machine_singleton &instance();

public: /* export */
	void transition(machine::state next);
	void loop();
	void shutdown();

private: /* load */
	void load_hardware();
	void load_map();
	void load_stdout();
	void load_stderr();

private: /* constraint */
	machine_singleton();
	machine_singleton(const machine_singleton &) = default;
	machine_singleton &operator=(const machine_singleton &) = default;
};