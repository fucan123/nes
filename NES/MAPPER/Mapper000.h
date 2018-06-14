#pragma once
#include "Mapper.h"

class Mapper000 : public Mapper {
public:
	Mapper000(NES* p) : Mapper(p) {};
	void RESET();
};