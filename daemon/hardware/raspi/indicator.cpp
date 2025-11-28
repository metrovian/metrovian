#include "daemon/hardware/raspi/indicator.h"
#include "core/predefined.h"

indicator_raspi::indicator_raspi() {
	chip_ = gpiod_chip_open_by_name("gpiochip0");
	if (chip_ == nullptr) {
		LOG_CONDITION(gpiod_chip_open_by_name == nullptr);
		return;
	}

	line_ = gpiod_chip_get_line(chip_, 17);
	if (line_ == nullptr) {
		LOG_CONDITION(gpiod_chip_get_line == nullptr);
		gpiod_chip_close(chip_);
		return;
	}

	int ret = gpiod_line_request_output(line_, "indicator", 0);
	if (ret < 0) {
		LOG_CONDITION(gpiod_line_request_output < 0);
		gpiod_chip_close(chip_);
		return;
	}
}

indicator_raspi::~indicator_raspi() {
	if (state_.load() != 0) {
		state_.store(2);
	}

	while (state_.load() != 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		continue;
	}

	gpiod_line_set_value(line_, 0);
	gpiod_line_release(line_);
	gpiod_chip_close(chip_);
}

void indicator_raspi::set(indicator::state state) {
	static auto output = [&](uint8_t tick) {
		uint8_t value = tick;
		state_.store(1);
		while (state_.load() == 1) {
			if (value > (tick * 2)) {
				value /= (tick * 2);
			}

			gpiod_line_set_value(line_, value++ / tick);
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}

		state_.store(0);
	};

	if (state_.load() != 0) {
		state_.store(2);
	}

	while (state_.load() != 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		continue;
	}

	switch (state) {
	case indicator::state::off:
		gpiod_line_set_value(line_, 0);
		break;
	case indicator::state::on:
		gpiod_line_set_value(line_, 1);
		break;
	case indicator::state::blink_slow:
		std::thread([&]() { output(20); }).detach();
		break;
	case indicator::state::blink_fast:
		std::thread([&]() { output(5); }).detach();
		break;
	default:
		LOG_ARGUMENT(state);
		break;
	}

	return;
}