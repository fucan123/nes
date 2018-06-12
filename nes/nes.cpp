#include "NES.h"

NES::NES(char* filename) {
	try {
		if (!(cpu = new CPU(this)))
			throw "ÉêÇëCPUÄÚ´æÊ§°Ü£¡";
		if (!(ppu = new PPU(this)))
			throw "ÉêÇëPPUÄÚ´æÊ§°Ü£¡";
		if (!(rom = new ROM))
			throw "ÉêÇëROMÄÚ´æÊ§°Ü£¡";
		if (!rom->Open(filename))
			throw "ÓÎÏ·ÎÄ¼þ´ò¿ªÊ§°Ü£¡";
		if (!(mapper = CreateMapper(rom->GetMapperNo()))) {
			char str[64];
			sprintf(str, "²»Ö§³ÖµÄMapperºÅ(%d)£¡", rom->GetMapperNo());
			throw str;
		}
		int no = rom->GetMapperNo();
		NES_HEADER* header = rom->GetHeader();

		MMC_INIT();
		mapper->RESET();
		cpu->RESET();
		ppu->load(CHRRom, no == 2 ? 0 : 0x2000, header->Control1 & 0x01);
	}
	catch (char* msg) {

	}
}