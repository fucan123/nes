#pragma once
#include "Mapper.h"

class Mapper004 : public Mapper {
protected:
	BYTE reg[8];
public:
	Mapper004(NES* p) : Mapper(p) {};
	void RESET();
	void Write(WORD addr, BYTE data);
};