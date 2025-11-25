#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include <atomic>
#include <condition_variable>
#include <map>
#include <mutex>
#include <queue>
#include <thread>
#include <fstream>
#include <sstream>
#include <string>
#include <stdint.h>
#include <spdlog/spdlog.h>

class decompression_abstract {
public: /* overload */
	virtual void decompress(const std::string &path) = 0;
	virtual void decompress(const std::string &path, const std::string &record) = 0;

public: /* abstract */
	virtual ~decompression_abstract() {}
};