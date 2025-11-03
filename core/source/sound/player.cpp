#include "sound/player.h"
#include "property.h"
#include "predefined.h"

int8_t sound_player::open() {
	LOG_ENTER();
	if (channel_ == 0) {
		LOG_CONDITION(channel_ == 0);
		LOG_EXIT();
		return -1;
	} else if (sample_rate_ == 0) {
		LOG_CONDITION(sample_rate_ == 0);
		LOG_EXIT();
		return -2;
	}

	if (snd_pcm_open(
		&handle_,
		CONFIG_STRING("alsa", "player", "name").c_str(),
		SND_PCM_STREAM_PLAYBACK,
		0) < 0) {
		LOG_CONDITION(snd_pcm_open < 0);
		LOG_EXIT();
		return -3;
	}

	if (snd_pcm_set_params(
		handle_,
		SND_PCM_FORMAT_S16_LE,
		SND_PCM_ACCESS_RW_INTERLEAVED,
		channel_,
		sample_rate_,
		1,
		5000) < 0) {
		snd_pcm_drain(handle_);
		snd_pcm_close(handle_);
		LOG_CONDITION(snd_pcm_set_params < 0);
		LOG_EXIT();
		return -4;
	}

	LOG_EXIT();
	return 0;
}

int8_t sound_player::close() {
	LOG_ENTER();
	if (handle_ != nullptr) {
		snd_pcm_drain(handle_);
		snd_pcm_close(handle_);
		handle_ = nullptr;
	}

	LOG_EXIT();
	return 0;
}

void sound_player::consume(std::vector<int16_t> &pcm) {
	int16_t *data = pcm.data();
	uint64_t write = pcm.size() / channel_;
	while (write > 0) {
		snd_pcm_sframes_t frames = snd_pcm_writei(handle_, data, write);
		if (frames == -EPIPE) {
			snd_pcm_prepare(handle_);
			LOG_WARN(snd_pcm_writei == -EPIPE);
			continue;
		} else if (frames < 0) {
			LOG_CONDITION(snd_pcm_writei < 0);
			break;
		}

		data += frames * channel_;
		write -= frames;
	}

	return;
}