#pragma once

#include "../stdafx.h"
#include "CPU.h"
#include "PPU.h"
#include "ROM.h"
#include "MAPPER/MapperFactory.h"


class NES {
public:
	CPU*	cpu;
	PPU*	ppu;
	ROM*	rom;
	Mapper*	mapper;
public:

	void Init();
	void Exec();
};