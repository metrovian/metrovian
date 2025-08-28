#include "decompression/abstract.h"
#include "predefined.h"

void decompression_abstract::push(std::vector<uint8_t> &payload) {
	std::unique_lock<std::mutex> lock(queue_mutex_);
	queue_.push(payload);
	lock.unlock();
	queue_cvar_.notify_one();
	return;
}

void decompression_abstract::pop(std::vector<uint8_t> &payload) {
	std::unique_lock<std::mutex> lock(queue_mutex_);
	queue_cvar_.wait(lock, [&]() { return (queue_.empty() == 0) || (queue_state_.load() != 1); });
	if (queue_.empty() == 0) {
		payload = queue_.front();
		queue_.pop();
	}

	return;
}

void decompression_abstract::clear() {
	std::unique_lock<std::mutex> lock(queue_mutex_);
	while (queue_.empty() == 0) {
		queue_.pop();
	}

	return;
}

void decompression_abstract::decompress(const std::string &path) {
	LOG_ENTER();
	AVFormatContext *avformat_ctx = nullptr;
	if (avformat_open_input(&avformat_ctx, path.c_str(), nullptr, nullptr) != 0) {
		LOG_CONDITION(avformat_open_input != 0);
		LOG_EXIT();
		return;
	}

	if (avformat_find_stream_info(avformat_ctx, nullptr) < 0) {
		avformat_close_input(&avformat_ctx);
		LOG_CONDITION(avformat_find_stream_info < 0);
		LOG_EXIT();
		return;
	}

	int64_t stream_index = -1;
	for (uint32_t i = 0; i < avformat_ctx->nb_streams; ++i) {
		if (avformat_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
			stream_index = static_cast<int64_t>(i);
			break;
		}
	}

	if (stream_index < 0) {
		avformat_close_input(&avformat_ctx);
		LOG_CONDITION(codec_type != AVMEDIA_TYPE_AUDIO);
		LOG_EXIT();
		return;
	}

	AVCodecParameters *avcodec_params = avformat_ctx->streams[stream_index]->codecpar;
	if (open(avcodec_params) < 0) {
		avformat_close_input(&avformat_ctx);
		LOG_CONDITION(open < 0);
		LOG_EXIT();
		return;
	}

	queue_state_.store(1);
	producer_thread_ = std::thread([&]() {
		AVPacket *avpacket = av_packet_alloc();
		while (av_read_frame(avformat_ctx, avpacket) >= 0 && queue_state_.load() == 1) {
			if (avpacket->stream_index == stream_index) {
				std::vector<uint8_t> payload(avpacket->data, avpacket->data + avpacket->size);
				std::vector<uint8_t> pcm = decompression(payload, avcodec_params);
				if (pcm.size() == 0) {
					break;
				}

				push(pcm);
			}

			av_packet_unref(avpacket);
		}

		av_packet_free(&avpacket);
		queue_state_.store(2);
		queue_cvar_.notify_one();
		return;
	});

	consumer_thread_ = std::thread([&]() {
		snd_pcm_t *pcm_handle = nullptr;
		snd_pcm_hw_params_t *pcm_params = nullptr;
		int32_t retcode = snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
		if (retcode < 0) {
			LOG_CONDITION(snd_pcm_open < 0);
			return;
		}

		snd_pcm_hw_params_malloc(&pcm_params);
		snd_pcm_hw_params_any(pcm_handle, pcm_params);
		snd_pcm_hw_params_set_access(pcm_handle, pcm_params, SND_PCM_ACCESS_RW_INTERLEAVED);
		snd_pcm_hw_params_set_format(pcm_handle, pcm_params, SND_PCM_FORMAT_S16_LE);
		snd_pcm_hw_params_set_channels(pcm_handle, pcm_params, avcodec_params->ch_layout.nb_channels);
		snd_pcm_hw_params_set_rate(pcm_handle, pcm_params, static_cast<uint32_t>(avcodec_params->sample_rate), 0);
		snd_pcm_hw_params(pcm_handle, pcm_params);
		snd_pcm_hw_params_free(pcm_params);
		snd_pcm_prepare(pcm_handle);
		while (queue_state_.load() != 0) {
			std::vector<uint8_t> pcm;
			pop(pcm);
			if (pcm.size() == 0) {
				break;
			}

			const int16_t *pcm_data = reinterpret_cast<const int16_t *>(pcm.data());
			size_t pcm_frames = pcm.size() / (avcodec_params->ch_layout.nb_channels * sizeof(int16_t));
			while (pcm_frames > 0) {
				retcode = snd_pcm_writei(pcm_handle, pcm_data, pcm_frames);
				if (retcode == -EPIPE) {
					snd_pcm_prepare(pcm_handle);
					LOG_CONDITION(snd_pcm_writei == -EPIPE);
					continue;
				} else if (retcode < 0) {
					LOG_CONDITION(snd_pcm_writei < 0);
					break;
				}

				pcm_data += retcode * avcodec_params->ch_layout.nb_channels;
				pcm_frames -= retcode;
			}
		}

		snd_pcm_drain(pcm_handle);
		snd_pcm_close(pcm_handle);
		return;
	});

	producer_thread_.join();
	consumer_thread_.join();
	queue_state_.store(0);
	close(avcodec_params);
	avformat_close_input(&avformat_ctx);
	LOG_EXIT();
	return;
}

decompression_abstract::decompression_abstract() {
	queue_state_.store(0);
	queue_cvar_.notify_one();
}

decompression_abstract::~decompression_abstract() {
	queue_state_.store(0);
	queue_cvar_.notify_one();
	clear();
}