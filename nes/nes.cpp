#include "NES.h"

NES::NES(char* filename) {
	try {
		if (!(cpu = new CPU(this)))
			throw "申请CPU内存失败！";
		if (!(ppu = new PPU(this)))
			throw "申请PPU内存失败！";
		if (!(rom = new ROM))
			throw "申请ROM内存失败！";
		if (!rom->Open(filename))
			throw "游戏文件打开失败！";
		if (!(mapper = CreateMapper(rom->GetMapperNo(), this))) {
			char str[64];
			sprintf(str, "不支持的Mapper号(%d)！", rom->GetMapperNo());
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
		CString str(msg);
		MessageBox(NULL, str, L"错误提示！", MB_OK);
	}
}