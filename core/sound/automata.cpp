#include "core/sound/automata.h"
#include "core/property.h"
#include "core/predefined.h"

automata_singleton &automata_singleton::instance() {
	static automata_singleton instance_;
	return instance_;
}

automata_singleton::automata_singleton() {
	LOG_ENTER();
	if (snd_seq_open(
		&handle_,
		"default",
		SND_SEQ_OPEN_OUTPUT,
		SND_SEQ_NONBLOCK) < 0) {
		LOG_CONDITION(snd_seq_open < 0);
		LOG_EXIT();
		return;
	}

	snd_seq_set_client_name(handle_, "Automata");
	if (snd_seq_create_simple_port(
		handle_,
		"Automata Port-0",
		SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ,
		SND_SEQ_PORT_TYPE_APPLICATION) < 0) {
		LOG_CONDITION(snd_seq_create_simple_port < 0);
		LOG_EXIT();
		return;
	}

	LOG_EXIT();
	return;
}

automata_singleton::~automata_singleton() {
	LOG_ENTER();
	if (handle_ != nullptr) {
		snd_seq_close(handle_);
		handle_ = nullptr;
	}

	LOG_EXIT();
	return;
}