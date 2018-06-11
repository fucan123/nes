#pragma once
#include "mapper.h"

class Mapper2: public Mapper {
	void REST();
	virtual	void Write(WORD addr, BYTE data);
};