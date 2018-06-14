#pragma once
#include "Mapper.h"

class Mapper003 : public Mapper {
public:
	Mapper003(NES* p) : Mapper(p) {};
	void RESET();
	void Write(WORD addr, BYTE data);
};