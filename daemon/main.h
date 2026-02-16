#pragma once
#include "daemon/machine/state/abstract.h"
#include "core/synthesis/abstract.h"

class machine_singleton {
public:
	friend class state_setup;
	friend class state_synthesis;
	friend class state_performance;

public:
	static machine_singleton &instance();

public:
	machine_singleton(const machine_singleton &) = delete;
	machine_singleton(machine_singleton &&) = delete;
	machine_singleton &operator=(const machine_singleton &) = delete;
	machine_singleton &operator=(machine_singleton &&) = delete;
	void loop();
	void shutdown();
	void panic();

private:
	static void handle_setup(const std::function<void(void)> handler);
	static void handle_terminate(int);

private:
	~machine_singleton() = default;
	machine_singleton();
	void transition(const machine::state next);
	void setup(const nlohmann::ordered_json &preset);
	void setup();
	void synthesize();
	void perform();

private:
	static inline std::function<void(void)> handler_ = nullptr;

private:
	std::unique_ptr<synthesis_abstract> core_ = nullptr;
	std::unordered_map<machine::state, std::unique_ptr<state_abstract>> smap_;
};