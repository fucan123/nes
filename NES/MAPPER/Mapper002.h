#pragma once
#include "Mapper.h"

class Mapper002: public Mapper {
public:
	void RESET();
	void Write(WORD addr, BYTE data);
};