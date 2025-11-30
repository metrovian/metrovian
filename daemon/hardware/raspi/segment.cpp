#include "daemon/hardware/raspi/segment.h"
#include "core/predefined.h"

segment_raspi::segment_raspi() {
	chip_ = gpiod_chip_open("/dev/gpiochip0");
	if (chip_ == nullptr) {
		LOG_CONDITION(gpiod_chip_open_by_name == nullptr);
		return;
	}

	std::vector<struct gpiod_line *> lines;
	std::vector<unsigned int> pins = {7, 8, 9, 10, 11, 12, 13, 19, 20, 21};
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

	line_a_ = lines[0];
	line_b_ = lines[1];
	line_c_ = lines[2];
	line_d_ = lines[3];
	line_e_ = lines[4];
	line_f_ = lines[5];
	line_g_ = lines[6];
	line1_ = lines[7];
	line2_ = lines[8];
	line3_ = lines[9];
	if (gpiod_line_request_output(line_a_, "seg_a", 0) < 0 ||
	    gpiod_line_request_output(line_b_, "seg_b", 0) < 0 ||
	    gpiod_line_request_output(line_c_, "seg_c", 0) < 0 ||
	    gpiod_line_request_output(line_d_, "seg_d", 0) < 0 ||
	    gpiod_line_request_output(line_e_, "seg_e", 0) < 0 ||
	    gpiod_line_request_output(line_f_, "seg_f", 0) < 0 ||
	    gpiod_line_request_output(line_g_, "seg_g", 0) < 0 ||
	    gpiod_line_request_output(line1_, "seg_1", 0) < 0 ||
	    gpiod_line_request_output(line2_, "seg_2", 0) < 0 ||
	    gpiod_line_request_output(line3_, "seg_3", 0) < 0) {
		for (const auto clear : lines) {
			gpiod_line_release(clear);
			continue;
		}

		gpiod_chip_close(chip_);
		LOG_CONDITION(gpiod_line_request_input < 0);
		return;
	}

	std::thread([&]() {
		static uint8_t phase = 0;
		static uint8_t numbers[10][7] = {
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

		state_.store(1);
		while (state_.load() == 1) {
			uint16_t knob = knob_.load();
			if (phase == 3) {
				phase = 0;
			}

			switch (phase++) {
			case 0:
				gpiod_line_set_value(line1_, 1);
				gpiod_line_set_value(line2_, 0);
				gpiod_line_set_value(line3_, 0);
				knob %= 10;
				break;
			case 1:
				gpiod_line_set_value(line1_, 0);
				gpiod_line_set_value(line2_, 1);
				gpiod_line_set_value(line3_, 0);
				knob /= 10;
				knob %= 10;
				break;
			case 2:
				gpiod_line_set_value(line1_, 0);
				gpiod_line_set_value(line2_, 0);
				gpiod_line_set_value(line3_, 1);
				knob /= 100;
				break;
			default:
				break;
			}

			gpiod_line_set_value(line_a_, numbers[knob][0]);
			gpiod_line_set_value(line_b_, numbers[knob][1]);
			gpiod_line_set_value(line_c_, numbers[knob][2]);
			gpiod_line_set_value(line_d_, numbers[knob][3]);
			gpiod_line_set_value(line_e_, numbers[knob][4]);
			gpiod_line_set_value(line_f_, numbers[knob][5]);
			gpiod_line_set_value(line_g_, numbers[knob][6]);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		state_.store(0);
	}).detach();
}

segment_raspi::~segment_raspi() {
	if (state_.load() != 0) {
		state_.store(2);
	}

	while (state_.load() != 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		continue;
	}

	gpiod_line_set_value(line_a_, 0);
	gpiod_line_set_value(line_b_, 0);
	gpiod_line_set_value(line_c_, 0);
	gpiod_line_set_value(line_d_, 0);
	gpiod_line_set_value(line_e_, 0);
	gpiod_line_set_value(line_f_, 0);
	gpiod_line_set_value(line_g_, 0);
	gpiod_line_set_value(line1_, 0);
	gpiod_line_set_value(line2_, 0);
	gpiod_line_set_value(line3_, 0);
	gpiod_line_release(line_a_);
	gpiod_line_release(line_b_);
	gpiod_line_release(line_c_);
	gpiod_line_release(line_d_);
	gpiod_line_release(line_e_);
	gpiod_line_release(line_f_);
	gpiod_line_release(line_g_);
	gpiod_line_release(line1_);
	gpiod_line_release(line2_);
	gpiod_line_release(line3_);
	gpiod_chip_close(chip_);
}

void segment_raspi::set(uint8_t num) {
	knob_.store(num);
	return;
}
