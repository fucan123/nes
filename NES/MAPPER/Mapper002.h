#pragma once
#include "Mapper.h"

class Mapper002: public Mapper {
public:
	Mapper002(NES* p) : Mapper(p) {};
	void RESET();
	void Write(WORD addr, BYTE data);
};