#pragma once
#include "daemon/state/abstract.h"

class context_singleton {
protected: /* context */
	machine::state state_ = machine::state::setup;
	nlohmann::ordered_json preset_;
	nlohmann::ordered_json presets_;

public: /* main setter */
	void set_state(const machine::state &state);
	void set_presets(const nlohmann::ordered_json &presets);

public: /* api getter */
	machine::state get_state();
	nlohmann::ordered_json get_presets();

public: /* api setter */
	void set_preset(uint8_t id);

public: /* main getter */
	nlohmann::ordered_json get_preset();

public: /* instance */
	static context_singleton &instance();

private: /* constraint */
	context_singleton();
	context_singleton(const context_singleton &) = default;
	context_singleton &operator=(const context_singleton &) = default;
};