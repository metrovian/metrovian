#include "core/sound/automata.h"
#include "core/property.h"
#include "core/predefined.h"

void automata_singleton::thread_playback() {
	int tick_delta = 0;
	int tick_prev = 0;
	int tick_tempo = 500000;
	smf_rewind(smf_);
	state_.store(1);
	while (state_.load() == 1) {
		smf_event_t *smf_event = smf_get_next_event(smf_);
		if (smf_event == NULL) {
			break;
		}

		if (smf_event->midi_buffer != nullptr) {
			tick_delta = smf_event->time_pulses - tick_prev;
			tick_prev = smf_event->time_pulses;
			if (tick_delta > 0) {
				usleep(static_cast<int64_t>(tick_delta * tick_tempo) / smf_->ppqn);
			}

			if (smf_event->midi_buffer[0] == 0xFF) {
				if (smf_event->midi_buffer[1] == 0x51 &&
				    smf_event->midi_buffer[2] == 0x03) {
					tick_tempo =
					    (smf_event->midi_buffer[3] << 16) |
					    (smf_event->midi_buffer[4] << 8) |
					    (smf_event->midi_buffer[5]);
				}
			} else {
				int status = smf_event->midi_buffer[0] & 0xF0;
				if (status != noteon_ &&
				    status != noteoff_) {
					continue;
				}

				int channel = smf_event->midi_buffer[0] & 0x0F;
				int note = smf_event->midi_buffer[1];
				int velocity = smf_event->midi_buffer[2];
				snd_seq_event_t seq_event;
				snd_seq_ev_clear(&seq_event);
				snd_seq_ev_set_source(&seq_event, port_);
				snd_seq_ev_set_subs(&seq_event);
				snd_seq_ev_set_direct(&seq_event);
				if (status == noteon_ &&
				    velocity > 0) {
					snd_seq_ev_set_noteon(&seq_event, channel, note, velocity);
				} else {
					snd_seq_ev_set_noteoff(&seq_event, channel, note, velocity);
				}

				snd_seq_event_output_direct(handle_, &seq_event);
			}
		}
	}

	state_.store(0);
	return;
}

int8_t automata_singleton::open(std::string name) {
	LOG_ENTER();
	smf_ = smf_load(name.c_str());
	if (smf_ == nullptr) {
		LOG_CONDITION(smf_load == nullptr);
		LOG_EXIT();
		return -1;
	}

	std::thread(&automata_singleton::thread_playback, this).detach();
	LOG_EXIT();
	return 0;
}

int8_t automata_singleton::close() {
	LOG_ENTER();
	state_.store(2);
	while (state_.load() != 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		continue;
	}

	if (smf_ != nullptr) {
		smf_delete(smf_);
		smf_ = nullptr;
	}

	LOG_EXIT();
	return 0;
}

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
	port_ = snd_seq_create_simple_port(
	    handle_,
	    "Automata Port-0",
	    SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ,
	    SND_SEQ_PORT_TYPE_APPLICATION);

	if (port_ < 0) {
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