#include "Mapper004.h"
#include "../PPU.h"
#include "../NES.h"

void Mapper004::RESET() {
	memset(reg, 0, sizeof(reg));
	Set_PRGRom_32K_Bank(0, 1, PRGRom_8K_Size - 2, PRGRom_8K_Size - 1);
	Set_CHRRom_8K_Bank(0);
}

void Mapper004::Write(WORD addr, BYTE data) {
	switch (addr) {
	case 0x8000:
		reg[0] = data;
		break;
	case 0x8001: {
		byte xv = data & 0x80 ? 0x1000 : 0;
		byte d02 = data & 0x03;
		if (d02 == 0)
			Set_CHRRom_2K_Bank(0, data);
		else if (d02 == 1)
			Set_CHRRom_2K_Bank(2, data);
		else if (d02 == 2)
			Set_CHRRom_1K_Bank(4, data);
		else if (d02 == 3)
			Set_CHRRom_1K_Bank(5, data);
		else if (d02 == 4)
			Set_CHRRom_1K_Bank(6, data);
		else if (d02 == 5)
			Set_CHRRom_1K_Bank(7, data);
		else if (d02 == 6)
			Set_PRGRom_8K_Bank(0, data);
		else if (d02 == 7)
			Set_PRGRom_8K_Bank(2, data);
		break;
	}
	case 0xA000:
		nes->ppu->SetMirroring((data ^ 0x01) & 0x01);
		break;
	case 0xA001:
		break;
	default:
		break;
	}
}