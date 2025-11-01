#include "sound/writer.h"
#include "predefined.h"

int8_t sound_writer::open() {
	LOG_ENTER();
	output_.open(path_, std::ios::binary);
	if (output_.is_open() == false) {
		LOG_CONDITION(output_.is_open() == false);
		return -1;
	}

	uint16_t audio_format = 1;     // PCM
	uint16_t bits_per_sample = 16; // AV_SAMPLE_FMT_S16
	uint32_t byte_rate = sample_rate_ * channel_ * 2;
	uint16_t block_align = channel_ * 2;
	uint32_t subchunk_size = 16;
	chunk_ = 36;
	data_ = 0;
	output_.write("RIFF", 4);
	output_.write(reinterpret_cast<const char *>(&chunk_), 4);
	output_.write("WAVE", 4);
	output_.write("fmt ", 4);
	output_.write(reinterpret_cast<const char *>(&subchunk_size), 4);
	output_.write(reinterpret_cast<const char *>(&audio_format), 2);
	output_.write(reinterpret_cast<const char *>(&channel_), 2);
	output_.write(reinterpret_cast<const char *>(&sample_rate_), 4);
	output_.write(reinterpret_cast<const char *>(&byte_rate), 4);
	output_.write(reinterpret_cast<const char *>(&block_align), 2);
	output_.write(reinterpret_cast<const char *>(&bits_per_sample), 2);
	output_.write("data", 4);
	output_.write(reinterpret_cast<const char *>(&data_), 4);
	LOG_EXIT();
	return 0;
}

int8_t sound_writer::close() {
	LOG_ENTER();
	output_.seekp(4, std::ios::beg);
	output_.write(reinterpret_cast<const char *>(&chunk_), 4);
	output_.seekp(40, std::ios::beg);
	output_.write(reinterpret_cast<const char *>(&data_), 4);
	output_.close();
	LOG_EXIT();
	return 0;
}

void sound_writer::consume(std::vector<int16_t> &pcm) {
	output_.write(reinterpret_cast<const char *>(pcm.data()), pcm.size() * 2);
	data_ += static_cast<uint32_t>(pcm.size() * 2);
	chunk_ += static_cast<uint32_t>(pcm.size() * 2);
	return;
}