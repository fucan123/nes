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
		nescfg = new NESCONFIG;
		nescfg->CpuClock = 1773447.0f;
		nescfg->FrameRate = 50;

		if (!(cpu = new CPU(this)))
			throw "����CPU�ڴ�ʧ�ܣ�";
		if (!(ppu = new PPU(this)))
			throw "����PPU�ڴ�ʧ�ܣ�";
		if (!(apu = new APU(this)))
			throw "����PPU�ڴ�ʧ�ܣ�";
		if (!(rom = new ROM))
			throw "����ROM�ڴ�ʧ�ܣ�";
		if (!rom->Open(filename))
			throw "��Ϸ�ļ���ʧ�ܣ�";
		if (!(mapper = CreateMapper(rom->GetMapperNo(), this))) {
			char str[64];
			sprintf(str, "��֧�ֵ�Mapper��(%d)��", rom->GetMapperNo());
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
		MessageBox(NULL, str, L"������ʾ��", MB_OK);
	}
}

BYTE NES::Read(WORD addr) {
	if (addr >= 0x4000 && addr <= 0x4014) {
		return 0x40;
	}
	else if (addr == 0x4015) {
		return REG[0x15];
	}
	else if (addr >= 0x4016 && addr <= 0x4017) {
		return REG[addr & 0x17];
	}
	else {
		return 0;
	}
}

void NES::Write(WORD addr, BYTE value) {
	if (addr >= 0x4000 && addr <= 0x4017) {
		REG[addr & 0x1F] = value;

		if (addr == 0x4014) { // DMA��ʽ���Ƶ�����RAM
			addr = value * 0x100;
			ppu->dmaSRAM(&CPU_MEM_BANK[addr >> 13][addr & 0x1FFF]);
		}

		apu->Write(addr, value);

		if (addr == 0x4015) {
			for (BYTE i = 0; i < 4; i++) {
				W_IF(apu->CNT[i].Sound, !(value & (1 << i)), 0);
			}
		}
	}
}

int NES::GetError() {
	return error;
}