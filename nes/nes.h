#pragma once

#include "../stdafx.h"
#include "MMU.h"
#include "CPU.h"
#include "PPU.h"
#include "ROM.h"

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

	BYTE    REG[0x17 + 1];
public:
	NES(char* filename);
	BYTE Read(WORD addr);
	void Write(WORD addr, BYTE value);

	void Exec();
};