#include "daemon/main.h"

machine_singleton &machine_singleton::instance() {
	static machine_singleton instance_;
	return instance_;
}

int32_t main(int argc, char **argv) {
	machine_singleton::instance();
	return 0;
}