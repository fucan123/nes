#pragma once

#include "../../stdafx.h"
#include "../mmu.h"

class Mapper {
	virtual void REST() = 0;
	virtual	void Write(WORD addr, BYTE data) {}
};