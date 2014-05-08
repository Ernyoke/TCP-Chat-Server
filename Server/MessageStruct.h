#pragma once
#include <stdint.h>

struct MessageStruct{
	uint32_t type;
    uint32_t from;
    uint32_t dest;
    uint32_t islast;
    uint32_t dataSize;
    char fileName[50];
    char data[8000];
};



