#pragma once

#include "../../stdafx.h"
#include "../MMU.h"

class NES;

class Mapper {
public:
	NES* nes;
public:
	Mapper(NES* p);
	virtual void RESET() = 0;
	virtual	void Write(WORD addr, BYTE data) {}
};