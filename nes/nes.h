#pragma once

#include "../stdafx.h"
#include "MMU.h"
#include "CPU.h"
#include "PPU.h"
#include "ROM.h"
#include "MAPPER/MapperFactory.h"

class CPU;
class PPU;
class ROM;
class Mapper;

class NES {
public:
	CPU*	cpu;
	PPU*	ppu;
	ROM*	rom;
	Mapper*	mapper;
public:
	NES(char* filename);

	void Exec();
};