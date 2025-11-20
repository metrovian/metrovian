#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <vector>
#include <map>
#include <memory>

class knob_abstract {
protected: /* abstract */
	virtual uint8_t read() = 0;
};