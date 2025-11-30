#include "daemon/hardware/raspi/knob.h"
#include "core/predefined.h"

knob_raspi::knob_raspi() {
	LOG_ENTER();
	chip_ = gpiod_chip_open("/dev/gpiochip0");
	if (chip_ == nullptr) {
		LOG_CONDITION(gpiod_chip_open_by_name == nullptr);
		return;
	}

	std::vector<struct gpiod_line *> lines;
	std::vector<unsigned int> pins = {22, 23, 24};
	for (uint8_t i = 0; i < pins.size(); ++i) {
		struct gpiod_line *line = gpiod_chip_get_line(chip_, pins[i]);
		if (line == nullptr) {
			for (const auto clear : lines) {
				gpiod_line_release(clear);
				continue;
			}

			gpiod_chip_close(chip_);
			LOG_CONDITION(gpiod_chip_get_line == nullptr);
			return;
		} else {
			lines.push_back(line);
		}
	}

	line1_ = lines[0];
	line2_ = lines[1];
	line3_ = lines[2];
	if (gpiod_line_request_input(line1_, "knob_key") < 0 ||
	    gpiod_line_request_input(line2_, "knob_s1") < 0 ||
	    gpiod_line_request_input(line3_, "knob_s2") < 0) {
		for (const auto clear : lines) {
			gpiod_line_release(clear);
			continue;
		}

		gpiod_chip_close(chip_);
		LOG_CONDITION(gpiod_line_request_input < 0);
		return;
	}

	segment_ = std::make_unique<segment_raspi>();
	LOG_EXIT();
}

knob_raspi::~knob_raspi() {
	LOG_ENTER();
	segment_.reset();
	state_.store(2);
	stop();
	gpiod_line_release(line1_);
	gpiod_line_release(line2_);
	gpiod_line_release(line3_);
	gpiod_chip_close(chip_);
	LOG_EXIT();
}

void knob_raspi::start() {
	LOG_ENTER();
	std::thread([this]() {
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
				segment_->set(num);
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

	LOG_EXIT();
	return;
}

void knob_raspi::stop() {
	LOG_ENTER();
	while (state_.load() != 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		continue;
	}

	value_.store(value_.load() & 0xFF);
	LOG_EXIT();
	return;
}

uint16_t knob_raspi::update() {
	return value_.load();
}
