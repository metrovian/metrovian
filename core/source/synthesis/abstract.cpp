#include "synthesis/abstract.h"
#include "property.h"
#include "predefined.h"

int8_t synthesis::muxer::keysize(uint64_t note) {
	key_.resize(note);
	return 0;
}

int8_t synthesis::muxer::keysample(std::vector<int16_t> &pcm, uint64_t note) {
	key_[note].sample_ = pcm;
	return 0;
}

int8_t synthesis::muxer::keydown(uint8_t vel, uint64_t note) {
	std::lock_guard lock(mutex_);
	key_[note].vel_ = vel;
	key_[note].pos_ = 0;
	key_[note].active_ = 1;
	return 0;
}

int8_t synthesis::muxer::keyrel(uint8_t vel, uint64_t note) {
	std::lock_guard lock(mutex_);
	key_[note].vel_ = vel;
	key_[note].pos_ = 0;
	key_[note].active_ = 0;
	return 0;
}

int8_t synthesis::muxer::sustain(uint8_t down) {
	std::lock_guard lock(mutex_);
	sustain_ = down > 63 ? 1 : 0;
	return 0;
}

std::vector<int16_t> synthesis::muxer::mux(uint64_t len) {
	std::lock_guard lock(mutex_);
	std::vector<int16_t> pcm(len, 0);
	for (auto &note : key_) {
		if (note.active_ == 0) {
			continue;
		}

		for (uint64_t i = 0; i < len; ++i) {
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

int8_t synthesis::sequencer::open(synthesis::muxer *ptr) {
	LOG_ENTER();
	snd_seq_t *handle = nullptr;
	if (snd_seq_open(
		&handle,
		property_singleton::instance().parse({"alsa", "sequencer", "name"}).c_str(),
		SND_SEQ_OPEN_INPUT,
		0) < 0) {
		LOG_CONDITION(snd_seq_open < 0);
		return -1;
	}

	int32_t port = snd_seq_create_simple_port(
	    handle,
	    "InputPort",
	    SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE,
	    SND_SEQ_PORT_TYPE_MIDI_GENERIC | SND_SEQ_PORT_TYPE_APPLICATION);

	int32_t client = std::stoull(property_singleton::instance().parse({"alsa", "sequencer", "client"}));
	int32_t cport = std::stoull(property_singleton::instance().parse({"alsa", "sequencer", "port"}));
	if (port < 0) {
		snd_seq_close(handle);
		LOG_CONDITION(snd_seq_create_simple_port < 0);
		return -2;
	}

	if (snd_seq_connect_from(handle, port, client, cport) < 0) {
		snd_seq_close(handle);
		LOG_CONDITION(snd_seq_connect_from < 0);
		return -3;
	}

	muxer_ = ptr;
	state_.store(1);
	while (state_.load() != 0) {
		snd_seq_event_t *event = nullptr;
		if (snd_seq_event_input(handle, &event) < 0) {
			continue;
		} else if (event != nullptr) {
			if (event->type == SND_SEQ_EVENT_NOTEON) {
				muxer_->keydown(
				    event->data.note.velocity,
				    event->data.note.note);
			} else if (event->type == SND_SEQ_EVENT_NOTEOFF) {
				muxer_->keyrel(
				    event->data.note.velocity,
				    event->data.note.note);
			}
		}
	}

	snd_seq_close(handle);
	LOG_EXIT();
	return 0;
}

int8_t synthesis::sequencer::close() {
	state_.store(0);
	return 0;
}

int8_t synthesis::player::open(synthesis::muxer *ptr) {
	LOG_ENTER();
	snd_pcm_t *handle = nullptr;
	if (snd_pcm_open(
		&handle,
		property_singleton::instance().parse({"alsa", "player", "name"}).c_str(),
		SND_PCM_STREAM_PLAYBACK,
		0) < 0) {
		LOG_CONDITION(snd_pcm_open < 0);
		return -1;
	}

	if (snd_pcm_set_params(
		handle,
		SND_PCM_FORMAT_S16_LE,
		SND_PCM_ACCESS_RW_INTERLEAVED,
		std::stoul(property_singleton::instance().parse({"alsa", "player", "channel"})),
		std::stoul(property_singleton::instance().parse({"alsa", "player", "sample-rate"})),
		1,
		0) < 0) {
		snd_pcm_drain(handle);
		snd_pcm_close(handle);
		LOG_CONDITION(snd_pcm_set_params < 0);
		return -1;
	}

	muxer_ = ptr;
	state_.store(1);
	while (state_.load() != 0) {
		std::vector<int16_t> output = muxer_->mux(32);
		int16_t *data = output.data();
		uint64_t write = output.size();
		while (write > 0) {
			snd_pcm_sframes_t frames = snd_pcm_writei(handle, data, write);
			if (frames == -EPIPE) {
				snd_pcm_prepare(handle);
				LOG_WARN(snd_pcm_writei == -EPIPE);
				continue;
			} else if (frames < 0) {
				LOG_CONDITION(snd_pcm_writei < 0);
				break;
			}

			data += frames * std::stoul(property_singleton::instance().parse({"alsa", "player", "channel"}));
			write -= frames;
		}
	}

	snd_pcm_drain(handle);
	snd_pcm_close(handle);
	LOG_EXIT();
	return 0;
}

int8_t synthesis::player::close() {
	state_.store(0);
	return 0;
}

int8_t synthesis_abstract::synthesize() {
	synthesis(
	    std::stoull(property_singleton::instance().parse({"synthesis", "note"})),
	    std::stoull(property_singleton::instance().parse({"synthesis", "period"})));

	std::thread sequencer = std::thread([&]() { sequencer_.open(&muxer_); });
	std::thread player = std::thread([&]() { player_.open(&muxer_); });
	sequencer.join();
	player.join();
	return 0;
}