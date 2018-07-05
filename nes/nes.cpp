#include "../stdafx.h"
#include "MMU.h"
#include "CPU.h"
#include "PPU.h"
#include "APU.h"
#include "ROM.h"
#include "NES.h"
#include "MAPPER/MapperFactory.h"

NES::NES(char* filename) {
	error = 0;
	try {
		if (!(cpu = new CPU(this)))
			throw "ÉêÇëCPUÄÚ´æÊ§°Ü£¡";
		if (!(ppu = new PPU(this)))
			throw "ÉêÇëPPUÄÚ´æÊ§°Ü£¡";
		if (!(apu = new APU(this)))
			throw "ÉêÇëPPUÄÚ´æÊ§°Ü£¡";
		if (!(rom = new ROM))
			throw "ÉêÇëROMÄÚ´æÊ§°Ü£¡";
		if (!rom->Open(filename))
			throw "ÓÎÏ·ÎÄ¼ş´ò¿ªÊ§°Ü£¡";
		if (!(mapper = CreateMapper(rom->GetMapperNo(), this))) {
			char str[64];
			sprintf(str, "²»Ö§³ÖµÄMapperºÅ(%d)£¡", rom->GetMapperNo());
			throw str;
		}
		int no = rom->GetMapperNo();
		NES_HEADER* header = rom->GetHeader();
		//MessageBox(NULL, L"x", L"", MB_OK);
		MMC_INIT();
		mapper->RESET();
		cpu->RESET();
		ppu->load(CHRRom, no == 2 ? 0 : 0x2000, header->Control1 & 0x01);
	}
	catch (char* msg) {
		error = 1;
		CString str(msg);
		MessageBox(NULL, str, L"´íÎóÌáÊ¾£¡", MB_OK);
	}
}

BYTE NES::Read(WORD addr) {
	if (addr >= 0x4000 && addr <= 0x4014) {
		return 0x40;
	}
	else if (addr >= 0x4015 && addr <= 0x4017) {
		return REG[addr & 0x17];
	}
	else {
		return 0;
	}
}

void NES::Write(WORD addr, BYTE value) {
	if (addr >= 0x4000 && addr <= 0x4017) {
		REG[addr & 0x17] = value;
	}
}

int NES::GetError() {
	return error;
}