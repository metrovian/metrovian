#pragma once
#include "daemon/state/abstract.h"

class context_singleton {
protected: /* context */
	machine::state state_ = machine::state::setup;
	nlohmann::ordered_json presets_;
	nlohmann::ordered_json preset_;
	uint8_t id_ = 0;
	uint64_t progress_ = 0;

public: /* api getter */
	machine::state apiget_state();
	nlohmann::ordered_json apiget_presets();
	uint8_t apiget_preset();
	uint64_t apiget_progress();
	uint64_t apiget_notes();

public: /* api setter */
	void apiset_preset(uint8_t id);

public: /* main getter */
	nlohmann::ordered_json get_preset();

public: /* main setter */
	void set_state(const machine::state &state);
	void set_presets(const nlohmann::ordered_json &presets);
	void set_progress(uint64_t progress);

public: /* instance */
	static context_singleton &instance();

private: /* constraint */
	context_singleton();
	context_singleton(const context_singleton &) = default;
	context_singleton &operator=(const context_singleton &) = default;
};