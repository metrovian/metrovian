#include "core/sound/player.h"
#include "core/property.h"
#include "core/predefined.h"

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

	char plug_hw[64] = {0};
	char card_hw[32] = {0};
	int card = -1;
	int dev = -1;
	while ((snd_card_next(&card) >= 0) && (card >= 0)) {
		snd_ctl_t *ctl = nullptr;
		snprintf(card_hw, sizeof(card_hw), "hw:%d", card);
		if (snd_ctl_open(&ctl, card_hw, 0) < 0) {
			continue;
		}

		while ((snd_ctl_pcm_next_device(ctl, &dev) >= 0) && (dev >= 0)) {
			snd_pcm_info_t *info = nullptr;
			snd_pcm_info_alloca(&info);
			snd_pcm_info_set_device(info, dev);
			snd_pcm_info_set_subdevice(info, 0);
			snd_pcm_info_set_stream(info, SND_PCM_STREAM_PLAYBACK);
			if (snd_ctl_pcm_info(ctl, info) < 0) {
				continue;
			}

			snprintf(plug_hw, sizeof(plug_hw), "plughw:%d,%d", card, dev);
			if (snd_pcm_open(
				&handle_,
				plug_hw,
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
	}

	LOG_CONDITION(snd_card_next < 0);
	LOG_EXIT();
	return -5;
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