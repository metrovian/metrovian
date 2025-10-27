#include "reconstruction/decompression/audio.h"
#include "property.h"
#include "predefined.h"

void decompression_audio::decompress(const std::string &path) {
	LOG_ENTER();
	std::thread producer = std::thread([&]() { open(path); });
	std::thread consumer = std::thread([&]() {
		usleep(CONFIG_UINT32("decompression", "playback-delay"));
		snd_pcm_t *handle = nullptr;
		snd_pcm_hw_params_t *params = nullptr;
		if (snd_pcm_open(
			&handle,
			CONFIG_STRING("alsa", "player", "name").c_str(),
			SND_PCM_STREAM_PLAYBACK,
			0) < 0) {
			LOG_CONDITION(snd_pcm_open < 0);
			return;
		}

		snd_pcm_hw_params_malloc(&params);
		snd_pcm_hw_params_any(handle, params);
		snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
		snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
		snd_pcm_hw_params_set_channels(handle, params, channel_);
		snd_pcm_hw_params_set_rate(handle, params, sample_rate_, 0);
		snd_pcm_hw_params(handle, params);
		snd_pcm_hw_params_free(params);
		snd_pcm_prepare(handle);
		while (queue_state_.load() != 0) {
			std::vector<uint8_t> pcm;
			pop(pcm);
			if (pcm.size() == 0) {
				break;
			}

			const int16_t *data = reinterpret_cast<const int16_t *>(pcm.data());
			uint64_t write = pcm.size() / (channel_ * sizeof(int16_t));
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

				data += frames * channel_;
				write -= frames;
			}
		}

		snd_pcm_drain(handle);
		snd_pcm_close(handle);
		return;
	});

	producer.join();
	consumer.join();
	queue_state_.store(0);
	close();
	LOG_EXIT();
	return;
}

void decompression_audio::decompress(const std::string &path, const std::string &record) {
	LOG_ENTER();
	std::thread producer = std::thread([&]() { open(path); });
	std::thread consumer = std::thread([&]() {
		usleep(CONFIG_UINT32("decompression", "playback-delay"));
		std::ofstream wav_record(record, std::ios::binary);
		if (wav_record.is_open() == false) {
			LOG_CONDITION(wav_record.is_open() == false);
			return;
		}

		uint16_t audio_format = 1;     // PCM
		uint16_t bits_per_sample = 16; // AV_SAMPLE_FMT_S16
		uint32_t byte_rate = sample_rate_ * channel_ * 2;
		uint16_t block_align = channel_ * 2;
		uint32_t data_size = 0;
		uint32_t chunk_size = 36;
		uint32_t subchunk_size = 16;
		auto write_header = [&]() {
			wav_record.seekp(0, std::ios::beg);
			wav_record.write("RIFF", 4);
			wav_record.write(reinterpret_cast<const char *>(&chunk_size), 4);
			wav_record.write("WAVE", 4);
			wav_record.write("fmt ", 4);
			wav_record.write(reinterpret_cast<const char *>(&subchunk_size), 4);
			wav_record.write(reinterpret_cast<const char *>(&audio_format), 2);
			wav_record.write(reinterpret_cast<const char *>(&channel_), 2);
			wav_record.write(reinterpret_cast<const char *>(&sample_rate_), 4);
			wav_record.write(reinterpret_cast<const char *>(&byte_rate), 4);
			wav_record.write(reinterpret_cast<const char *>(&block_align), 2);
			wav_record.write(reinterpret_cast<const char *>(&bits_per_sample), 2);
			wav_record.write("data", 4);
			wav_record.write(reinterpret_cast<const char *>(&data_size), 4);
		};

		write_header();
		while (queue_state_.load() != 0) {
			std::vector<uint8_t> pcm;
			pop(pcm);
			if (pcm.empty()) {
				break;
			}

			wav_record.write(reinterpret_cast<const char *>(pcm.data()), pcm.size());
			data_size += static_cast<uint32_t>(pcm.size());
			chunk_size += static_cast<uint32_t>(pcm.size());
		}

		write_header();
		wav_record.close();
		return;
	});

	producer.join();
	consumer.join();
	queue_state_.store(0);
	close();
	LOG_EXIT();
	return;
}
