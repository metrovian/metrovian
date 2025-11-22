#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <vector>
#include <map>
#include <memory>

class segment_abstract {
protected: /* abstract */
	virtual void write(uint16_t number) = 0;
};