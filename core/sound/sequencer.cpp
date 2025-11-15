#include "core/sound/sequencer.h"
#include "core/property.h"
#include "core/predefined.h"

void sound_sequencer::thread_event() {
	state_.store(1);
	while (state_.load() != 0) {
		snd_seq_event_t *event = nullptr;
		if (snd_seq_event_input(handle_, &event) < 0) {
			continue;
		} else if (event != nullptr) {
			mutex_.lock();
			if (event->type == SND_SEQ_EVENT_NOTEON) {
				key_[event->data.note.note].vel_ = event->data.note.velocity;
				key_[event->data.note.note].pos_ = 0;
				key_[event->data.note.note].active_ = 1;
			} else if (event->type == SND_SEQ_EVENT_NOTEOFF) {
				key_[event->data.note.note].vel_ = event->data.note.velocity;
				key_[event->data.note.note].pos_ = 0;
				key_[event->data.note.note].active_ = 0;
			}

			mutex_.unlock();
		}
	}

	return;
}

void sound_sequencer::resize(uint64_t note) {
	key_.resize(note);
	return;
}

void sound_sequencer::resample(uint64_t note, std::vector<int16_t> &pcm) {
	key_[note].sample_ = pcm;
	return;
}

int8_t sound_sequencer::open() {
	LOG_ENTER();
	if (snd_seq_open(
		&handle_,
		CONFIG_STRING("alsa", "sequencer", "name").c_str(),
		SND_SEQ_OPEN_INPUT,
		0) < 0) {
		LOG_CONDITION(snd_seq_open < 0);
		LOG_EXIT();
		return -1;
	}

	snd_seq_addr_t address = {0, 0};
	if (snd_seq_parse_address(
		handle_,
		&address,
		CONFIG_STRING("alsa", "sequencer", "port").c_str()) < 0) {
		snd_seq_close(handle_);
		LOG_CONDITION(snd_seq_parse_address < 0);
		LOG_EXIT();
		return -2;
	};

	len_ = CONFIG_UINT64("alsa", "sequencer", "mux-length");
	if (snd_seq_connect_from(
		handle_,
		snd_seq_create_simple_port(
		    handle_,
		    "InputPort",
		    SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE,
		    SND_SEQ_PORT_TYPE_MIDI_GENERIC | SND_SEQ_PORT_TYPE_APPLICATION),
		address.client,
		address.port) < 0) {
		snd_seq_close(handle_);
		LOG_CONDITION(snd_seq_connect_from < 0);
		LOG_EXIT();
		return -3;
	}

	std::thread(&sound_sequencer::thread_event, this).detach();
	LOG_EXIT();
	return 0;
}

int8_t sound_sequencer::close() {
	LOG_ENTER();
	state_.store(0);
	if (handle_ != nullptr) {
		snd_seq_close(handle_);
		handle_ = nullptr;
	}

	LOG_EXIT();
	return 0;
}

std::vector<int16_t> sound_sequencer::produce() {
	std::lock_guard lock(mutex_);
	std::vector<int16_t> pcm(len_, 0);
	for (sound::note &note : key_) {
		if (note.active_ == 0) {
			continue;
		}

		for (uint64_t i = 0; i < len_; ++i) {
			if (note.pos_ > note.sample_.size() - 1) {
				note.pos_ = 0;
				note.active_ = 0;
				break;
			}

			pcm[i] += note.sample_[note.pos_++] * (note.vel_ / 128.0);
		}
	}

	return pcm;
}