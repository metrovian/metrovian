#include "daemon/hardware/raspi/knob.h"
#include "core/predefined.h"

void knob_raspi::start() {
	tcgetattr(STDIN_FILENO, &terminal_);
	termios terminal = terminal_;
	terminal.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &terminal);
	std::thread([this]() {
		uint8_t num = 0;
		state_.store(1);
		while (state_.load() == 1) {
			if (read(STDIN_FILENO, &num, 1) > 0) {
				if (std::isdigit(num)) {
					value_.store(static_cast<uint16_t>(num - '0'));
					state_.store(2);
				}
			}
		}

		state_.store(0);
		LOG_VALUE(static_cast<int32_t>(value_));
	}).detach();

	return;
}

void knob_raspi::stop() {
	while (state_.load() != 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		continue;
	}

	tcsetattr(STDIN_FILENO, TCSANOW, &terminal_);
	return;
}

uint16_t knob_raspi::update() {
	return value_.load();
}
