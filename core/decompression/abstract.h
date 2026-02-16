#pragma once
#include <string>

class decompression_abstract {
public:
	virtual ~decompression_abstract() = default;
	virtual void decompress(const std::string &path) = 0;
	virtual void decompress(const std::string &path, const std::string &record) = 0;
};