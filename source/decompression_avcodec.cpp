#include "decompression_avcodec.h"
#include "predefined.h"

int8_t decompression_avcodec::open(AVCodecParameters *params) {
	avcodec_ = const_cast<AVCodec *>(avcodec_find_decoder(params->codec_id));
	if (avcodec_ == nullptr) {
		LOG_CONDITION(avcodec_find_decoder == nullptr);
		return -1;
	}

	avcodec_ctx_ = avcodec_alloc_context3(avcodec_);
	if (avcodec_ctx_ == nullptr) {
		LOG_CONDITION(avcodec_alloc_context3 == nullptr);
		return -2;
	}

	if (avcodec_parameters_to_context(avcodec_ctx_, params) < 0) {
		avcodec_free_context(&avcodec_ctx_);
		LOG_CONDITION(avcodec_parameters_to_context < 0);
		return -3;
	}

	if (avcodec_open2(avcodec_ctx_, avcodec_, nullptr) < 0) {
		avcodec_free_context(&avcodec_ctx_);
		LOG_CONDITION(avcodec_open2 < 0);
		return -4;
	}

	return 0;
}

int8_t decompression_avcodec::close(AVCodecParameters *params) {
	avcodec_free_context(&avcodec_ctx_);
	return 0;
}

std::vector<uint8_t> decompression_avcodec::decompression(const std::vector<uint8_t> &payload, AVCodecParameters *params) {
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
	if (avcodec_send_packet(avcodec_ctx_, avpacket) == 0) {
		while (avcodec_receive_frame(avcodec_ctx_, avframe) == 0) {
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
