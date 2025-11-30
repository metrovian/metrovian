#include "daemon/hardware/raspi/knob.h"
#include "core/predefined.h"
#include "knob.h"

void knob_raspi::segment_set(uint8_t num, uint8_t count) {
	const static uint8_t numbers[10][7] = {
	    {0, 0, 0, 0, 0, 0, 1},
	    {1, 0, 0, 1, 1, 1, 1},
	    {0, 0, 1, 0, 0, 1, 0},
	    {0, 0, 0, 0, 1, 1, 0},
	    {1, 0, 0, 1, 1, 0, 0},
	    {0, 1, 0, 0, 1, 0, 0},
	    {0, 1, 0, 0, 0, 0, 0},
	    {0, 0, 0, 1, 1, 1, 1},
	    {0, 0, 0, 0, 0, 0, 0},
	    {0, 0, 1, 0, 0, 0, 0}};

	uint8_t phase = count % 3;
	switch (phase) {
	case 0:
		gpiod_line_set_value(seg1_, 1);
		gpiod_line_set_value(seg2_, 0);
		gpiod_line_set_value(seg3_, 0);
		num %= 10;
		break;
	case 1:
		gpiod_line_set_value(seg1_, 0);
		gpiod_line_set_value(seg2_, 1);
		gpiod_line_set_value(seg3_, 0);
		num /= 10;
		num %= 10;
		break;
	case 2:
		gpiod_line_set_value(seg1_, 0);
		gpiod_line_set_value(seg2_, 0);
		gpiod_line_set_value(seg3_, 1);
		num /= 100;
		break;
	default:
		break;
	}

	gpiod_line_set_value(seg_a_, numbers[num][0]);
	gpiod_line_set_value(seg_b_, numbers[num][1]);
	gpiod_line_set_value(seg_c_, numbers[num][2]);
	gpiod_line_set_value(seg_d_, numbers[num][3]);
	gpiod_line_set_value(seg_e_, numbers[num][4]);
	gpiod_line_set_value(seg_f_, numbers[num][5]);
	gpiod_line_set_value(seg_g_, numbers[num][6]);
	return;
}

knob_raspi::knob_raspi() {
	chip_ = gpiod_chip_open("/dev/gpiochip0");
	if (chip_ == nullptr) {
		LOG_CONDITION(gpiod_chip_open_by_name == nullptr);
		return;
	}

	std::vector<struct gpiod_line *> lines;
	std::vector<unsigned int> pins = {7, 8, 9, 10, 11, 12, 13, 19, 20, 21, 22, 23, 24};
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

	seg_a_ = lines[0];
	seg_b_ = lines[1];
	seg_c_ = lines[2];
	seg_d_ = lines[3];
	seg_e_ = lines[4];
	seg_f_ = lines[5];
	seg_g_ = lines[6];
	seg1_ = lines[7];
	seg2_ = lines[8];
	seg3_ = lines[9];
	line1_ = lines[10];
	line2_ = lines[11];
	line3_ = lines[12];
	if (gpiod_line_request_output(seg_a_, "seg_A", 0) < 0 ||
	    gpiod_line_request_output(seg_b_, "seg_B", 0) < 0 ||
	    gpiod_line_request_output(seg_c_, "seg_C", 0) < 0 ||
	    gpiod_line_request_output(seg_d_, "seg_D", 0) < 0 ||
	    gpiod_line_request_output(seg_e_, "seg_E", 0) < 0 ||
	    gpiod_line_request_output(seg_f_, "seg_F", 0) < 0 ||
	    gpiod_line_request_output(seg_g_, "seg_G", 0) < 0 ||
	    gpiod_line_request_output(seg1_, "seg_1", 0) < 0 ||
	    gpiod_line_request_output(seg2_, "seg_2", 0) < 0 ||
	    gpiod_line_request_output(seg3_, "seg_3", 0) < 0 ||
	    gpiod_line_request_input(line1_, "knob_key") < 0 ||
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
}

knob_raspi::~knob_raspi() {
	state_.store(2);
	stop();
	gpiod_line_release(seg_a_);
	gpiod_line_release(seg_b_);
	gpiod_line_release(seg_c_);
	gpiod_line_release(seg_d_);
	gpiod_line_release(seg_e_);
	gpiod_line_release(seg_f_);
	gpiod_line_release(seg_g_);
	gpiod_line_release(seg1_);
	gpiod_line_release(seg2_);
	gpiod_line_release(seg3_);
	gpiod_line_release(line1_);
	gpiod_line_release(line2_);
	gpiod_line_release(line3_);
	gpiod_chip_close(chip_);
}

void knob_raspi::start() {
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
				LOG_VALUE(static_cast<int32_t>(value_.load()));
			}

			if (key != key_pre) {
				value_.store(0x100 | num);
				state_.store(2);
				gpiod_line_set_value(seg_a_, 0);
				gpiod_line_set_value(seg_b_, 0);
				gpiod_line_set_value(seg_c_, 0);
				gpiod_line_set_value(seg_d_, 0);
				gpiod_line_set_value(seg_e_, 0);
				gpiod_line_set_value(seg_f_, 0);
				gpiod_line_set_value(seg_g_, 0);
				gpiod_line_set_value(seg1_, 0);
				gpiod_line_set_value(seg2_, 0);
				gpiod_line_set_value(seg3_, 0);
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

	value_.store(value_.load() & 0xFF);
	return;
}

uint16_t knob_raspi::update() {
	static uint8_t count = 0;
	uint16_t value = value_.load();
	segment_set(value, count++);
	return value;
}
