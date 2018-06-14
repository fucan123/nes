#pragma once
#include "mapper.h"

class Mapper001 : public Mapper {
public:
	Mapper001(NES* p) : Mapper(p) {};
	void RESET();
};