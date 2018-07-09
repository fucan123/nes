#pragma once

#define W_IF(var,c,v) { if(c){var=v;} }
#define W_IF2(var,c,v1,v2) { var=c?v1:v2; }

typedef struct tagNESCONFIG {
	FLOAT	BaseClock;		// NTSC:21477270.0  PAL:21281364.0
	FLOAT	CpuClock;		// NTSC: 1789772.5  PAL: 1773447.0

	INT	TotalScanlines;		// NTSC: 262  PAL: 312

	INT	ScanlineCycles;		// NTSC:1364  PAL:1362

	INT	HDrawCycles;		// NTSC:1024  PAL:1024
	INT	HBlankCycles;		// NTSC: 340  PAL: 338
	INT	ScanlineEndCycles;	// NTSC:   4  PAL:   2

	INT	FrameCycles;		// NTSC:29829.52  PAL:35468.94
	INT	FrameIrqCycles;		// NTSC:29829.52  PAL:35468.94

	INT	FrameRate;		// NTSC:60(59.94) PAL:50
	FLOAT	FramePeriod;		// NTSC:16.683    PAL:20.0
} NESCONFIG, *LPNESCONFIG;

class CPU;
class PPU;
class APU;
class ROM;
class Mapper;

class NES {
protected:
	int error;
public:
	CPU*	cpu;
	PPU*	ppu;
	APU*    apu;
	ROM*	rom;
	Mapper*	mapper;
	NESCONFIG* nescfg;

	BYTE    REG[0x17 + 1];
public:
	NES(char* filename);
	BYTE Read(WORD addr);
	void Write(WORD addr, BYTE value);

	void Exec();
	int  GetError();
};