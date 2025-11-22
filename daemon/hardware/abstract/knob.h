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
	virtual uint16_t read() = 0;
};