#include "daemon/hardware/raspi/knob.h"
#include "core/predefined.h"

knob_raspi::~knob_raspi() {
	state_.store(2);
	stop();
}

void knob_raspi::start() {
	std::thread([this]() {
		chip_ = gpiod_chip_open("/dev/gpiochip0");
		if (chip_ == nullptr) {
			LOG_CONDITION(gpiod_chip_open_by_name == nullptr);
			return;
		}

		line1_ = gpiod_chip_get_line(chip_, 22);
		if (line1_ == nullptr) {
			LOG_CONDITION(gpiod_chip_get_line == nullptr);
			gpiod_chip_close(chip_);
			return;
		}

		line2_ = gpiod_chip_get_line(chip_, 23);
		if (line2_ == nullptr) {
			LOG_CONDITION(gpiod_chip_get_line == nullptr);
			gpiod_line_release(line1_);
			gpiod_chip_close(chip_);
			return;
		}

		line3_ = gpiod_chip_get_line(chip_, 24);
		if (line2_ == nullptr) {
			LOG_CONDITION(gpiod_chip_get_line == nullptr);
			gpiod_line_release(line1_);
			gpiod_line_release(line2_);
			gpiod_chip_close(chip_);
			return;
		}

		if (gpiod_line_request_input(line1_, "knob_key") < 0 ||
		    gpiod_line_request_input(line2_, "knob_s1") < 0 ||
		    gpiod_line_request_input(line3_, "knob_s2") < 0) {
			LOG_CONDITION(gpiod_line_request_input < 0);
			gpiod_line_release(line1_);
			gpiod_line_release(line2_);
			gpiod_line_release(line3_);
			gpiod_chip_close(chip_);
			return;
		}

		int32_t key_pre = gpiod_line_get_value(line1_);
		int32_t s1_pre = gpiod_line_get_value(line2_);
		int32_t s2_pre = gpiod_line_get_value(line2_);
		state_.store(1);
		while (state_.load() == 1) {
			int32_t key = gpiod_line_get_value(line1_);
			int32_t s1 = gpiod_line_get_value(line2_);
			int32_t s2 = gpiod_line_get_value(line3_);
			uint16_t knob = value_.load();
			uint8_t num = knob & 0xFF;
			if (s1 != s1_pre &&
			    s1 == 1) {
				value_.store(s2_pre == 1 ? ++num : --num);
				LOG_VALUE(static_cast<int32_t>(value_.load()));
			}

			if (key != key_pre) {
				value_.store(0x100 | num);
				state_.store(2);
				LOG_VALUE(static_cast<int32_t>(num));
			}

			key_pre = key;
			s1_pre = s1;
			s2_pre = s2;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		state_.store(0);
	}).detach();

	return;
}

void knob_raspi::stop() {
	while (state_.load() != 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		continue;
	}

	gpiod_line_release(line1_);
	gpiod_line_release(line2_);
	gpiod_line_release(line3_);
	gpiod_chip_close(chip_);
	value_.store(value_.load() & 0xFF);
	return;
}

uint16_t knob_raspi::update() {
	return value_.load();
}
