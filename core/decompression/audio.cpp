#include "core/decompression/audio.h"
#include "core/property.h"
#include "core/predefined.h"

uint16_t decompression_producer::channel() {
	return channel_;
}

uint32_t decompression_producer::sample_rate() {
	return sample_rate_;
}

void decompression_producer::set_uri(const std::string &path) {
	LOG_ENTER();
	if (avformat_open_input(&avformat_ctx_, path.c_str(), nullptr, nullptr) != 0) {
		LOG_CONDITION(avformat_open_input != 0);
		LOG_EXIT();
		return;
	}

	if (avformat_find_stream_info(avformat_ctx_, nullptr) < 0) {
		avformat_close_input(&avformat_ctx_);
		LOG_CONDITION(avformat_find_stream_info < 0);
		LOG_EXIT();
		return;
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
		LOG_EXIT();
		return;
	}

	avcodec_params_ = avformat_ctx_->streams[stream_index]->codecpar;
	stream_ = static_cast<int32_t>(stream_index);
	channel_ = static_cast<uint16_t>(avcodec_params_->ch_layout.nb_channels);
	sample_rate_ = static_cast<uint32_t>(avcodec_params_->sample_rate);
	LOG_EXIT();
	return;
}

int8_t decompression_producer::open() {
	LOG_ENTER();
	if (avcodec_params_ == nullptr) {
		LOG_CONDITION(avcodec_params_ == nullptr);
		LOG_EXIT();
		return -1;
	}

	avcodec_ = const_cast<AVCodec *>(avcodec_find_decoder(avcodec_params_->codec_id));
	if (avcodec_ == nullptr) {
		avformat_close_input(&avformat_ctx_);
		LOG_CONDITION(avcodec_find_decoder == nullptr);
		LOG_EXIT();
		return -2;
	}

	avcodec_ctx_ = avcodec_alloc_context3(avcodec_);
	if (avcodec_ctx_ == nullptr) {
		avformat_close_input(&avformat_ctx_);
		LOG_CONDITION(avcodec_alloc_context3 == nullptr);
		LOG_EXIT();
		return -3;
	}

	if (avcodec_parameters_to_context(avcodec_ctx_, avcodec_params_) < 0) {
		avformat_close_input(&avformat_ctx_);
		avcodec_free_context(&avcodec_ctx_);
		LOG_CONDITION(avcodec_parameters_to_context < 0);
		LOG_EXIT();
		return -4;
	}

	if (avcodec_open2(avcodec_ctx_, avcodec_, nullptr) < 0) {
		avformat_close_input(&avformat_ctx_);
		avcodec_free_context(&avcodec_ctx_);
		LOG_CONDITION(avcodec_open2 < 0);
		LOG_EXIT();
		return -5;
	}

	LOG_EXIT();
	return 0;
}

int8_t decompression_producer::close() {
	LOG_ENTER();
	avformat_close_input(&avformat_ctx_);
	avcodec_free_context(&avcodec_ctx_);
	LOG_EXIT();
	return 0;
}

std::vector<int16_t> decompression_producer::produce() {
	std::vector<int16_t> pcm;
	AVFrame *avframe = av_frame_alloc();
	AVPacket *avpacket = av_packet_alloc();
	while (av_read_frame(avformat_ctx_, avpacket) == 0) {
		if (avpacket->stream_index == stream_) {
			break;
		}

		av_packet_unref(avpacket);
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
		return std::vector<int16_t>();
	} else if (swr_init(swr_ctx) < 0) {
		swr_free(&swr_ctx);
		av_frame_free(&avframe);
		av_packet_free(&avpacket);
		LOG_CONDITION(swr_init < 0);
		return std::vector<int16_t>();
	}

	if (avcodec_send_packet(avcodec_ctx_, avpacket) == 0) {
		while (avcodec_receive_frame(avcodec_ctx_, avframe) == 0) {
			int32_t rescale_samples =
			    av_rescale_rnd(
				swr_get_delay(swr_ctx, avcodec_params_->sample_rate) + avframe->nb_samples,
				avcodec_params_->sample_rate,
				avcodec_params_->sample_rate,
				AV_ROUND_UP);

			std::vector<int16_t> converted(rescale_samples * avcodec_params_->ch_layout.nb_channels);
			int32_t converted_samples =
			    swr_convert(
				swr_ctx,
				reinterpret_cast<uint8_t **>(&converted),
				rescale_samples,
				const_cast<const uint8_t **>(avframe->data),
				avframe->nb_samples);

			if (converted_samples > 0) {
				pcm.insert(
				    pcm.end(),
				    converted.begin(),
				    converted.begin() + converted_samples * avcodec_params_->ch_layout.nb_channels);
			}

			av_frame_unref(avframe);
		}
	}

	swr_free(&swr_ctx);
	av_frame_free(&avframe);
	av_packet_free(&avpacket);
	return pcm;
}

uint16_t decompression_audio::channel() {
	return dynamic_cast<decompression_producer *>(producer_.get())->channel();
}

uint32_t decompression_audio::sample_rate() {
	return dynamic_cast<decompression_producer *>(producer_.get())->sample_rate();
}

void decompression_audio::prepare(const std::string &path) {
	dynamic_cast<decompression_producer *>(producer_.get())->set_uri(path);
	return;
}

void decompression_audio::decompress(const std::string &path) {
	LOG_ENTER();
	create();
	prepare(path);
	consumer_ =
	    std::make_unique<sound_player>(
		channel(),
		sample_rate());

	run(sound::pipeline::common);
	LOG_EXIT();
	return;
}

void decompression_audio::decompress(const std::string &path, const std::string &record) {
	LOG_ENTER();
	create();
	prepare(path);
	consumer_ =
	    std::make_unique<sound_writer>(
		record,
		channel(),
		sample_rate());

	run(sound::pipeline::common);
	LOG_EXIT();
	return;
}

std::unique_ptr<sound_producer> decompression_audio::create_producer() {
	return std::make_unique<decompression_producer>();
}

std::unique_ptr<sound_consumer> decompression_audio::create_consumer() {
	return nullptr;
}