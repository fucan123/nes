#pragma once
#include "mapper.h"

class Mapper2: public Mapper {
public:
	void RESET();
	void Write(WORD addr, BYTE data);
};