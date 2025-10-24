#pragma once
#include "reconstruction/decompression/abstract.h"

class decompression_audio : public decompression_abstract {
protected: /* parameter */
	uint16_t channels_ = 0;
	uint32_t sample_rate_ = 0;

public: /* overload */
	virtual void decompress(const std::string &path) override final;
	virtual void decompress(const std::string &path, const std::string &record) override final;
};