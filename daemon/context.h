#pragma once
#include "daemon/machine/state/abstract.h"

#include <string>
#include <nlohmann/json.hpp>

class context_singleton {
public:
	friend class context_api;
	friend class context_machine;

public:
	static context_singleton &instance();

public:
	context_singleton(const context_singleton &) = delete;
	context_singleton(context_singleton &&) = delete;
	context_singleton &operator=(const context_singleton &) = delete;
	context_singleton &operator=(context_singleton &&) = delete;

private:
	~context_singleton() = default;
	context_singleton();

private:
	nlohmann::ordered_json presets_;
	nlohmann::ordered_json preset_;
	std::string mid_;
	std::atomic<machine::state> state_ = machine::state::setup;
	std::atomic<uint8_t> play_ = 0;
	std::atomic<uint8_t> id_ = 0;
	std::atomic<uint64_t> progress_ = 0;
};

class context_api {
public:
	static nlohmann::ordered_json read_presets();
	static nlohmann::ordered_json read_mids();
	static std::string read_mid();
	static uint8_t read_state();
	static uint8_t read_id();
	static uint64_t read_progress();
	static uint64_t read_notes();
	static void write_presets(const nlohmann::ordered_json &presets);
	static void write_preset(const uint8_t id);
};

class context_machine {
public:
	static nlohmann::ordered_json read_preset();
	static machine::state read_state();
	static uint8_t read_play();
	static void write_mid(const std::string mid);
	static void write_state(const machine::state state);
	static void write_play(const uint8_t play);
	static void write_progress(const uint64_t progress);
};