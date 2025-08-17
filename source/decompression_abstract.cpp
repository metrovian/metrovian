#include "decompression_abstract.h"
#include "predefined.h"

void decompression_abstract::push(std::vector<uint8_t> &payload) {
	std::unique_lock<std::mutex> lock(queue_mutex);
	queue.push(payload);
	lock.unlock();
	queue_cvar.notify_one();
	return;
}

void decompression_abstract::pop(std::vector<uint8_t> &payload) {
	std::unique_lock<std::mutex> lock(queue_mutex);
	queue_cvar.wait(lock, [&]() { return (queue.empty() == 0) || (queue_state.load() != 1); });
	if (queue.empty() == 0) {
		payload = queue.front();
		queue.pop();
	}

	return;
}

void decompression_abstract::clear() {
	std::unique_lock<std::mutex> lock(queue_mutex);
	while (queue.empty() == 0) {
		queue.pop();
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

	queue_state.store(1);
	producer_thread = std::thread([&]() {
		AVPacket *avpacket = av_packet_alloc();
		while (av_read_frame(avformat_ctx, avpacket) >= 0 && queue_state.load() == 1) {
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
		queue_state.store(2);
		queue_cvar.notify_one();
		return;
	});

	consumer_thread = std::thread([&]() {
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
		while (queue_state.load() != 0) {
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

	producer_thread.join();
	consumer_thread.join();
	queue_state.store(0);
	close(avcodec_params);
	avformat_close_input(&avformat_ctx);
	LOG_EXIT();
	return;
}

decompression_abstract::decompression_abstract() {
	queue_state.store(0);
	queue_cvar.notify_one();
}

decompression_abstract::~decompression_abstract() {
	queue_state.store(0);
	queue_cvar.notify_one();
	clear();
}

int8_t decompression_general::open(AVCodecParameters *params) {
	avcodec = const_cast<AVCodec *>(avcodec_find_decoder(params->codec_id));
	if (avcodec == nullptr) {
		LOG_CONDITION(avcodec_find_decoder == nullptr);
		return -1;
	}

	avcodec_ctx = avcodec_alloc_context3(avcodec);
	if (avcodec_ctx == nullptr) {
		LOG_CONDITION(avcodec_alloc_context3 == nullptr);
		return -2;
	}

	if (avcodec_parameters_to_context(avcodec_ctx, params) < 0) {
		avcodec_free_context(&avcodec_ctx);
		LOG_CONDITION(avcodec_parameters_to_context < 0);
		return -3;
	}

	if (avcodec_open2(avcodec_ctx, avcodec, nullptr) < 0) {
		avcodec_free_context(&avcodec_ctx);
		LOG_CONDITION(avcodec_open2 < 0);
		return -4;
	}

	return 0;
}

int8_t decompression_general::close(AVCodecParameters *params) {
	avcodec_free_context(&avcodec_ctx);
	return 0;
}

std::vector<uint8_t> decompression_general::decompression(const std::vector<uint8_t> &payload, AVCodecParameters *params) {
	std::vector<uint8_t> decompressed;
	AVFrame *avframe = av_frame_alloc();
	AVPacket *avpacket = av_packet_alloc();
	if (av_new_packet(avpacket, static_cast<int32_t>(payload.size())) < 0) {
		av_frame_free(&avframe);
		av_packet_free(&avpacket);
		LOG_CONDITION(av_new_packet < 0);
		return std::vector<uint8_t>();
	}

	SwrContext *swr_ctx = nullptr;
	swr_alloc_set_opts2(
	    &swr_ctx,
	    &params->ch_layout,
	    AV_SAMPLE_FMT_S16,
	    params->sample_rate,
	    &params->ch_layout,
	    static_cast<AVSampleFormat>(params->format),
	    params->sample_rate,
	    0,
	    nullptr);

	if (swr_ctx == nullptr) {
		av_frame_free(&avframe);
		av_packet_free(&avpacket);
		LOG_CONDITION(swr_ctx == nullptr);
		return std::vector<uint8_t>();
	} else if (swr_init(swr_ctx) < 0) {
		swr_free(&swr_ctx);
		av_frame_free(&avframe);
		av_packet_free(&avpacket);
		LOG_CONDITION(swr_init < 0);
		return std::vector<uint8_t>();
	}

	memcpy(avpacket->data, payload.data(), payload.size());
	if (avcodec_send_packet(avcodec_ctx, avpacket) == 0) {
		while (avcodec_receive_frame(avcodec_ctx, avframe) == 0) {
			int32_t rescale_samples =
			    av_rescale_rnd(
				swr_get_delay(swr_ctx, params->sample_rate) + avframe->nb_samples,
				params->sample_rate,
				params->sample_rate,
				AV_ROUND_UP);

			std::vector<uint8_t> converted(rescale_samples * params->ch_layout.nb_channels * 2);
			int32_t converted_samples =
			    swr_convert(
				swr_ctx,
				reinterpret_cast<uint8_t **>(&converted),
				rescale_samples,
				const_cast<const uint8_t **>(avframe->data),
				avframe->nb_samples);

			if (converted_samples > 0) {
				decompressed.insert(
				    decompressed.end(),
				    converted.begin(),
				    converted.begin() + converted_samples * params->ch_layout.nb_channels * 2);
			}

			av_frame_unref(avframe);
		}
	}

	swr_free(&swr_ctx);
	av_frame_free(&avframe);
	av_packet_free(&avpacket);
	return decompressed;
}
