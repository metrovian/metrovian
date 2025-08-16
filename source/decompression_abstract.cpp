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
	spdlog::trace("[enter] {}", __PRETTY_FUNCTION__);
	AVFormatContext *avformat_ctx = nullptr;
	if (avformat_open_input(&avformat_ctx, path.c_str(), nullptr, nullptr) != 0) {
		spdlog::error("decompression open failed: {}", path);
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		return;
	}

	if (avformat_find_stream_info(avformat_ctx, nullptr) < 0) {
		spdlog::error("decompression find stream information failed: {}", path);
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		avformat_close_input(&avformat_ctx);
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
		spdlog::error("decompression find audio stream failed: {}", path);
		spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
		avformat_close_input(&avformat_ctx);
		return;
	}

	AVCodecParameters *avcodec_params = avformat_ctx->streams[stream_index]->codecpar;
	queue_state.store(1);
	producer_thread = std::thread([&]() {
		AVPacket *avpacket = av_packet_alloc();
		while (av_read_frame(avformat_ctx, avpacket) >= 0) {
			if (avpacket->stream_index == stream_index) {
				std::vector<uint8_t> payload(avpacket->data, avpacket->data + avpacket->size);
				std::vector<uint8_t> raw = decompression(payload);
				push(raw);
			}

			av_packet_unref(avpacket);
		}

		av_packet_free(&avpacket);
		avformat_close_input(&avformat_ctx);
		queue_state.store(2);
	});

	consumer_thread = std::thread([&]() {
		while (queue_state.load() != 0) {
			std::vector<uint8_t> pcm;
			pop(pcm);
			if (!pcm.empty()) {
				spdlog::debug("decompressed: {}", pcm.size());
			} else {
				break;
			}
		}
	});

	producer_thread.join();
	consumer_thread.join();
	queue_state.store(0);
	spdlog::trace("[exit] {}", __PRETTY_FUNCTION__);
	return;
}

decompression_abstract::decompression_abstract() {
}

decompression_abstract::~decompression_abstract() {
}

std::vector<uint8_t> decompression_abstract::decompression(const std::vector<uint8_t> &payload) {
	return payload;
}