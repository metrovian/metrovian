#include "reconstruction/decompression/avcodec.h"
#include "predefined.h"

int8_t decompression_avcodec::open(const std::string &path) {
	if (avformat_open_input(&avformat_ctx_, path.c_str(), nullptr, nullptr) != 0) {
		LOG_CONDITION(avformat_open_input != 0);
		return -1;
	}

	if (avformat_find_stream_info(avformat_ctx_, nullptr) < 0) {
		avformat_close_input(&avformat_ctx_);
		LOG_CONDITION(avformat_find_stream_info < 0);
		return -2;
	}

	int64_t stream_index = -1;
	for (uint32_t i = 0; i < avformat_ctx_->nb_streams; ++i) {
		if (avformat_ctx_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
			stream_index = static_cast<int64_t>(i);
			break;
		}
	}

	if (stream_index < 0) {
		avformat_close_input(&avformat_ctx_);
		LOG_CONDITION(codec_type != AVMEDIA_TYPE_AUDIO);
		return -3;
	}

	avcodec_params_ = avformat_ctx_->streams[stream_index]->codecpar;
	avcodec_ = const_cast<AVCodec *>(avcodec_find_decoder(avcodec_params_->codec_id));
	if (avcodec_ == nullptr) {
		avformat_close_input(&avformat_ctx_);
		LOG_CONDITION(avcodec_find_decoder == nullptr);
		return -4;
	}

	avcodec_ctx_ = avcodec_alloc_context3(avcodec_);
	if (avcodec_ctx_ == nullptr) {
		avformat_close_input(&avformat_ctx_);
		LOG_CONDITION(avcodec_alloc_context3 == nullptr);
		return -5;
	}

	if (avcodec_parameters_to_context(avcodec_ctx_, avcodec_params_) < 0) {
		avformat_close_input(&avformat_ctx_);
		avcodec_free_context(&avcodec_ctx_);
		LOG_CONDITION(avcodec_parameters_to_context < 0);
		return -6;
	}

	channels_ = static_cast<uint16_t>(avcodec_params_->ch_layout.nb_channels);
	sample_rate_ = static_cast<uint32_t>(avcodec_params_->sample_rate);
	if (avcodec_open2(avcodec_ctx_, avcodec_, nullptr) < 0) {
		avformat_close_input(&avformat_ctx_);
		avcodec_free_context(&avcodec_ctx_);
		LOG_CONDITION(avcodec_open2 < 0);
		return -7;
	}

	AVPacket *avpacket = av_packet_alloc();
	queue_state_.store(1);
	while (av_read_frame(avformat_ctx_, avpacket) >= 0 && queue_state_.load() == 1) {
		if (avpacket->stream_index == stream_index) {
			std::vector<uint8_t> payload(avpacket->data, avpacket->data + avpacket->size);
			std::vector<uint8_t> pcm = decompression(payload);
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
	return 0;
}

int8_t decompression_avcodec::close() {
	avformat_close_input(&avformat_ctx_);
	avcodec_free_context(&avcodec_ctx_);
	return 0;
}

std::vector<uint8_t> decompression_avcodec::decompression(const std::vector<uint8_t> &payload) {
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
	    &avcodec_params_->ch_layout,
	    AV_SAMPLE_FMT_S16,
	    avcodec_params_->sample_rate,
	    &avcodec_params_->ch_layout,
	    static_cast<AVSampleFormat>(avcodec_params_->format),
	    avcodec_params_->sample_rate,
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
	if (avcodec_send_packet(avcodec_ctx_, avpacket) == 0) {
		while (avcodec_receive_frame(avcodec_ctx_, avframe) == 0) {
			int32_t rescale_samples =
			    av_rescale_rnd(
				swr_get_delay(swr_ctx, avcodec_params_->sample_rate) + avframe->nb_samples,
				avcodec_params_->sample_rate,
				avcodec_params_->sample_rate,
				AV_ROUND_UP);

			std::vector<uint8_t> converted(rescale_samples * avcodec_params_->ch_layout.nb_channels * 2);
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
				    converted.begin() + converted_samples * avcodec_params_->ch_layout.nb_channels * 2);
			}

			av_frame_unref(avframe);
		}
	}

	swr_free(&swr_ctx);
	av_frame_free(&avframe);
	av_packet_free(&avpacket);
	return decompressed;
}
