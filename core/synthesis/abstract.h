#pragma once
#include "core/sound/factory.h"

#include <functional>
#include <nlohmann/json.hpp>

class synthesis_abstract : public sound_factory {
public:
	virtual ~synthesis_abstract() = default;
	synthesis_abstract();
	synthesis_abstract(const nlohmann::ordered_json &preset);
	int synthesize();
	int perform();
	int panic();
	void callback_disconnect(std::function<void(void)> function);
	void callback_change(std::function<void(unsigned, int)> function);
	void callback_synthesis(std::function<void(uint64_t)> function);

protected:
	void set_envelope(const nlohmann::ordered_json &preset);
	void set_scale(uint16_t volume);
	void set_size(uint64_t note);
	void set_sample(uint64_t note, std::vector<int16_t> &pcm);
	virtual void synthesis(uint64_t min, uint64_t max, uint64_t period) = 0;

private:
	std::unique_ptr<sound_producer> create_producer() override final;
	std::unique_ptr<sound_consumer> create_consumer() override final;

protected:
	std::function<void(uint64_t)> on_synthesis_ = nullptr;
};