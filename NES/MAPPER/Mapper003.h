#pragma once
#include "Mapper.h"

class Mapper003 : public Mapper {
public:
	void RESET();
	void Write(WORD addr, BYTE data);
};