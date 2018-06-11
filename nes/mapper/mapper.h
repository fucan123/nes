#pragma once

#include "../../stdafx.h"
#include "../MMU.h"

class Mapper {
public:
	virtual void REST() = 0;
	virtual	void Write(WORD addr, BYTE data) {}
};