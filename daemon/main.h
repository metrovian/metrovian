#pragma once
#include "daemon/server-micro/api.h"
#include "daemon/server-micro/context.h"
#include "daemon/state/abstract.h"
#include "daemon/state/dummy.h"
#include "daemon/state/setup.h"
#include "daemon/state/synthesis.h"
#include "daemon/state/performance.h"
#include "core/synthesis/abstract.h"
#include "core/synthesis/add.h"

class machine_singleton {
protected: /* machine core */
	std::unique_ptr<synthesis_abstract> core_ = nullptr;

protected: /* machine map */
	std::atomic<machine::state> state_ = machine::state::none;
	std::unordered_map<machine::state, std::unique_ptr<state_abstract>> smap_;

protected: /* machine function */
	void transition(machine::state next);
	void setup(nlohmann::json preset);
	void setup();
	void synthesize();
	void perform();

protected: /* handler */
	static inline std::function<void(void)> handler_ = nullptr;

protected: /* handler setup */
	static void handle_setup(const std::function<void(void)> handler);
	static void handle_terminate(int);

protected: /* friend */
	friend class state_abstract;
	friend class state_setup;
	friend class state_synthesis;
	friend class state_performance;

public: /* instance */
	static machine_singleton &instance();

public: /* export */
	void loop();
	void shutdown();

private: /* load */
	void load_stdout();
	void load_stderr();
	void load_smap();
	void load_wmap();

private: /* constraint */
	machine_singleton();
	machine_singleton(const machine_singleton &) = default;
	machine_singleton &operator=(const machine_singleton &) = default;
};