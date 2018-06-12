#include "NES.h"

NES::NES(char* filename) {
	try {
		if (!(cpu = new CPU(this)))
			throw "����CPU�ڴ�ʧ�ܣ�";
		if (!(ppu = new PPU(this)))
			throw "����PPU�ڴ�ʧ�ܣ�";
		if (!(rom = new ROM))
			throw "����ROM�ڴ�ʧ�ܣ�";
		if (!rom->Open(filename))
			throw "��Ϸ�ļ���ʧ�ܣ�";
		if (!(mapper = CreateMapper(rom->GetMapperNo()))) {
			char str[64];
			sprintf(str, "��֧�ֵ�Mapper��(%d)��", rom->GetMapperNo());
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