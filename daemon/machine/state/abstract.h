#pragma once
#include <stdint.h>
#include <atomic>

namespace machine {
enum class state : uint8_t {
	none = 0,
	setup = 1,
	synthesis = 2,
	performance = 3,
	shutdown = 4,
};
}; // namespace machine

class state_abstract {
public:
	virtual ~state_abstract() = default;
	virtual void enter() = 0;
	virtual void update() = 0;
};

class state_none final : public state_abstract {
public:
	void enter() override {}
	void update() override {}
};

class state_setup final : public state_abstract {
public:
	void enter() override;
	void update() override;
};

class state_synthesis final : public state_abstract {
public:
	void enter() override;
	void update() override;

private:
	std::atomic<uint8_t> complete_ = 0;
};

class state_performance final : public state_abstract {
public:
	void enter() override;
	void update() override;

private:
	std::atomic<uint8_t> complete_ = 0;
};