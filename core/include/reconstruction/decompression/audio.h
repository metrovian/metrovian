#pragma once
#include "reconstruction/decompression/abstract.h"

class decompression_audio : public decompression_abstract {
public: /* overload */
	virtual void decompress(const std::string &path) override final;
	virtual void decompress(const std::string &path, const std::string &record) override final;
};