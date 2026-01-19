#include "core/sound/sequencer.h"
#include "core/property.h"
#include "core/predefined.h"

void sound_sequencer::thread_event() {
	state_.store(1);
	while (state_.load() == 1) {
		snd_seq_event_t *event = nullptr;
		if (snd_seq_event_input(handle_, &event) < 0) {
			continue;
		} else if (event != nullptr) {
			mutex_.lock();
			switch (event->type) {
			case SND_SEQ_EVENT_NOTEON:
				if (event->data.note.velocity > 0) {
					key_[event->data.note.note].vel_ = event->data.note.velocity;
					key_[event->data.note.note].pos_enter_ = 0;
					key_[event->data.note.note].pos_ = 0;
					key_[event->data.note.note].active_ = 1;
					key_[event->data.note.note].state_ = sound::state::attack;
				} else {
					key_[event->data.note.note].pos_enter_ = key_[event->data.note.note].pos_;
					key_[event->data.note.note].state_ = sound::state::release;
				}
				break;
			case SND_SEQ_EVENT_NOTEOFF:
				key_[event->data.note.note].pos_enter_ = key_[event->data.note.note].pos_;
				key_[event->data.note.note].state_ = sound::state::release;
				break;
			case SND_SEQ_EVENT_CONTROLLER:
				if (on_change_ != nullptr) {
					on_change_(
					    event->data.control.param,
					    event->data.control.value);
				}
				break;
			case SND_SEQ_EVENT_CLIENT_EXIT:
			case SND_SEQ_EVENT_PORT_EXIT:
			case SND_SEQ_EVENT_PORT_UNSUBSCRIBED:
				if (on_disconnect_ != nullptr) {
					on_disconnect_();
				}
				break;
			default:
				break;
			}

			mutex_.unlock();
		}
	}

	state_.store(0);
	return;
}

double sound_sequencer::calc_envelope(sound::note &note) {
	switch (note.state_) {
	case sound::state::sustain:
		return sustain_;

	case sound::state::attack:
		if (note.pos_ < attack_) {
			return static_cast<double>(note.pos_) / attack_;
		} else {
			note.pos_enter_ += attack_;
			note.state_ = sound::state::decay;
			return 1.000E+0;
		}
	case sound::state::decay:
		if (note.pos_ < note.pos_enter_ + decay_) {
			return 1.000E+0 - ((note.pos_ - note.pos_enter_) * sustain_) / decay_;
		} else {
			note.pos_enter_ += decay_;
			note.state_ = sound::state::sustain;
			return sustain_;
		}
	case sound::state::release:
		if (note.pos_ < note.pos_enter_ + release_) {
			return sustain_ - ((note.pos_ - note.pos_enter_) * sustain_) / release_;
		} else {
			note.pos_enter_ = 0;
			note.pos_ = 0;
			note.active_ = 0;
			note.state_ = sound::state::none;
			return 0.000E+0;
		}
	default:
		return 0.000E+0;
	}
}

void sound_sequencer::set_envelope(double sustain, double attack, double decay, double release) {
	sustain_ = sustain;
	attack_ = static_cast<uint64_t>(sample_rate_ * attack);
	decay_ = static_cast<uint64_t>(sample_rate_ * decay);
	release_ = static_cast<uint64_t>(sample_rate_ * release);
	return;
}

void sound_sequencer::set_scale(uint16_t volume) {
	volume_ = volume;
	return;
}

void sound_sequencer::set_size(uint64_t note) {
	key_.resize(note);
	return;
}

void sound_sequencer::set_sample(uint64_t note, std::vector<int16_t> &pcm) {
	key_[note].sample_ = pcm;
	return;
}

void sound_sequencer::callback_disconnect(std::function<void(void)> function) {
	on_disconnect_ = function;
	return;
}

void sound_sequencer::callback_change(std::function<void(unsigned, int)> function) {
	on_change_ = function;
	return;
}

int8_t sound_sequencer::open() {
	LOG_ENTER();
	if (snd_seq_open(
		&handle_,
		CONFIG_STRING("alsa", "sequencer", "name").c_str(),
		SND_SEQ_OPEN_INPUT,
		SND_SEQ_NONBLOCK) < 0) {
		LOG_CONDITION(snd_seq_open < 0);
		LOG_EXIT();
		return -1;
	}

	len_ = CONFIG_UINT64("alsa", "sequencer", "mux-length");
	uint8_t ports = 0;
	snd_seq_client_info_t *cinfo = nullptr;
	snd_seq_port_info_t *pinfo = nullptr;
	snd_seq_client_info_alloca(&cinfo);
	snd_seq_port_info_alloca(&pinfo);
	while (snd_seq_query_next_client(handle_, cinfo) >= 0) {
		int32_t client = snd_seq_client_info_get_client(cinfo);
		if (client == 0 ||
		    client == 14) {
			continue;
		}

		snd_seq_port_info_set_client(pinfo, client);
		snd_seq_port_info_set_port(pinfo, -1);
		while (snd_seq_query_next_port(handle_, pinfo) >= 0) {
			if (snd_seq_connect_from(
				handle_,
				snd_seq_create_simple_port(
				    handle_,
				    "InputPort",
				    SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE,
				    SND_SEQ_PORT_TYPE_MIDI_GENERIC | SND_SEQ_PORT_TYPE_APPLICATION),
				client,
				snd_seq_port_info_get_port(pinfo)) == 0) {
				const char *name = snd_seq_port_info_get_name(pinfo);
				++ports;
				LOG_VALUE(name);
				continue;
			}
		}

		if (ports > 0) {
			std::thread(&sound_sequencer::thread_event, this).detach();
			LOG_EXIT();
			return 0;
		}
	}

	LOG_CONDITION(snd_seq_query_next_client < 0);
	LOG_EXIT();
	return -2;
}

int8_t sound_sequencer::close() {
	LOG_ENTER();
	state_.store(2);
	while (state_.load() != 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		continue;
	}

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
		double env = calc_envelope(note) / 16129.0;
		if (note.active_ != 0) {
			env *= note.vel_ * volume_;
			for (uint64_t i = 0; i < len_; ++i) {
				if (note.pos_ > note.sample_.size() - 1) {
					note.pos_ = 0;
					note.active_ = 0;
					break;
				}

				pcm[i] += note.sample_[note.pos_++] * env;
			}
		}
	}

	return pcm;
}