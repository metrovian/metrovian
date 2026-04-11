#pragma once
#include "core/sound/factory.h"

#include <functional>
#include <nlohmann/json.hpp>

class synthesis_abstract : public sound_factory {
public:
	virtual ~synthesis_abstract() = default;
	synthesis_abstract();
	synthesis_abstract(const nlohmann::ordered_json &preset);
	void setup();
	void synthesis();
	void panic();
	void callback_disconnect(std::function<void(void)> function);
	void callback_change(std::function<void(uint32_t, int32_t)> function);

protected:
	void set_envelope(const nlohmann::ordered_json &preset);
	void set_scale(uint16_t volume);
	void set_size(uint64_t note);
	void set_synthesis(std::function<int16_t(uint64_t, uint64_t)> function);
	virtual void setup(uint64_t max) = 0;

private:
	std::unique_ptr<sound_producer> create_producer() override final;
	std::unique_ptr<sound_consumer> create_consumer() override final;
};
