#include "MMU.h"

BYTE* CPU_MEM_BANK[8];  // 8K��, CPU���֧���ڴ�0x10000[0x0000-0xFFFF], ������Ҫ8�����浥Ԫ(0x10000/0x2000=8)
BYTE  RAM [8 * 1024];   // 2K-RAM ����4�ξ���, 0x0000-0x1FFF
BYTE  PRAM[8 * 1024];   // PPU�Ĵ���0x2000-0x2007, �����Ǿ���, 0x2000-0x3FFF
BYTE  ERAM[8 * 1024];   // 0x4000-0x401FΪ�Ĵ���, 0x4020-0x5FFFΪ��չROM, 0x4000-0x5FFFF
BYTE  SRAM[8 * 1024];   // ������SRAM, 0x6000-0x7FFF

BYTE* PPU_MEM_BANK[12]; // PPU 1K��
BYTE  CRAM[32 * 1024];  // PPU �ڴ� 32KB
BYTE  BGPAL[0x10];      // ������ɫ��
BYTE  SPPAL[0x10];      // �����ɫ��

BYTE* PRGRom;           // ����ROM��ַ
BYTE* CHRRom;           // ��ɫROM��ַ

BYTE PRGRom_8K_Size;    // PRGRom 8K ����
BYTE CHRRom_1K_Size;    // CHRRom 1K ����

void MMC_INIT() {
	memset(RAM, 0, sizeof(RAM));
	memset(PRAM, 0, sizeof(PRAM));
	memset(ERAM, 0, sizeof(ERAM));
	memset(SRAM, 0, sizeof(SRAM));
	memset(CRAM, 0, sizeof(CRAM));
	memset(BGPAL, 0, sizeof(BGPAL));
	memset(SPPAL, 0, sizeof(SPPAL));

	Set_PRGRom_Bank(0, RAM);
	Set_PRGRom_Bank(1, PRAM);
	Set_PRGRom_Bank(2, ERAM);
	Set_PRGRom_Bank(3, SRAM);

	Set_CRAM_4K_Bank(8, 2);
}

void Set_PRGRom_Bank(BYTE page, BYTE* ptr) {
	CPU_MEM_BANK[page] = ptr;
}

void Set_PRGRom_8K_Bank(BYTE page, BYTE bank) {
	CPU_MEM_BANK[page] = PRGRom + (bank * 0x2000); //ÿ��BANK 8K=0x2000
}

void Set_PRGRom_16K_Bank(BYTE page, BYTE bank) {
	Set_PRGRom_8K_Bank(page + 0, bank * 2 + 0);
	Set_PRGRom_8K_Bank(page + 1, bank * 2 + 1);
}

void Set_PRGRom_32K_Bank(BYTE bank) {
	Set_PRGRom_8K_Bank(4, bank * 4 + 0);
	Set_PRGRom_8K_Bank(5, bank * 4 + 1);
	Set_PRGRom_8K_Bank(6, bank * 4 + 2);
	Set_PRGRom_8K_Bank(7, bank * 4 + 3);
}

void Set_PRGRom_32K_Bank(BYTE bank0, BYTE bank1, BYTE bank2, BYTE bank3) {
	Set_PRGRom_8K_Bank(4, bank0);
	Set_PRGRom_8K_Bank(5, bank1);
	Set_PRGRom_8K_Bank(6, bank2);
	Set_PRGRom_8K_Bank(7, bank3);
}

void Set_CHRRom_Bank(BYTE page, BYTE* ptr) {
	PPU_MEM_BANK[page] = ptr;
}

void Set_CHRRom_1K_Bank(BYTE page, BYTE bank) {
	PPU_MEM_BANK[page] = CHRRom + (bank * 0x400); //ÿ��BANK 1K=0x400
}

void Set_CHRRom_2K_Bank(BYTE page, BYTE bank) {
	Set_CHRRom_1K_Bank(page + 0, bank * 2 + 0);
	Set_CHRRom_1K_Bank(page + 1, bank * 2 + 1);
}

void Set_CHRRom_4K_Bank(BYTE page, BYTE bank) {
	Set_CHRRom_1K_Bank(page + 0, bank * 4 + 0);
	Set_CHRRom_1K_Bank(page + 1, bank * 4 + 1);
	Set_CHRRom_1K_Bank(page + 2, bank * 4 + 2);
	Set_CHRRom_1K_Bank(page + 3, bank * 4 + 3);
}

void Set_CHRRom_8K_Bank(BYTE bank) {
	Set_CHRRom_1K_Bank(0, bank * 8 + 0);
	Set_CHRRom_1K_Bank(1, bank * 8 + 1);
	Set_CHRRom_1K_Bank(2, bank * 8 + 2);
	Set_CHRRom_1K_Bank(3, bank * 8 + 3);
	Set_CHRRom_1K_Bank(4, bank * 8 + 4);
	Set_CHRRom_1K_Bank(5, bank * 8 + 5);
	Set_CHRRom_1K_Bank(6, bank * 8 + 6);
	Set_CHRRom_1K_Bank(7, bank * 8 + 7);
}

void Set_CHRRom_8K_Bank(BYTE bank0, BYTE bank1, BYTE bank2, BYTE bank3, BYTE bank4, BYTE bank5, BYTE bank6, BYTE bank7) {
	Set_CHRRom_1K_Bank(0, bank0);
	Set_CHRRom_1K_Bank(1, bank1);
	Set_CHRRom_1K_Bank(2, bank2);
	Set_CHRRom_1K_Bank(3, bank3);
	Set_CHRRom_1K_Bank(4, bank4);
	Set_CHRRom_1K_Bank(5, bank5);
	Set_CHRRom_1K_Bank(6, bank6);
	Set_CHRRom_1K_Bank(7, bank7);
}

void Set_CRAM_Bank(BYTE page, BYTE* ptr) {
	PPU_MEM_BANK[page] = ptr;
}

void Set_CRAM_1K_Bank(BYTE page, BYTE bank) {
	PPU_MEM_BANK[page] = CRAM + (bank * 0x400); //ÿ��BANK 1K=0x400
}

void Set_CRAM_2K_Bank(BYTE page, BYTE bank) {
	Set_CRAM_1K_Bank(page + 0, bank * 2 + 0);
	Set_CRAM_1K_Bank(page + 1, bank * 2 + 1);
}

void Set_CRAM_4K_Bank(BYTE page, BYTE bank) {
	Set_CRAM_1K_Bank(page + 0, bank * 4 + 0);
	Set_CRAM_1K_Bank(page + 1, bank * 4 + 1);
	Set_CRAM_1K_Bank(page + 2, bank * 4 + 2);
	Set_CRAM_1K_Bank(page + 3, bank * 4 + 3);
}

void Set_CRAM_8K_Bank(BYTE bank) {
	Set_CRAM_1K_Bank(0, bank * 8 + 0);
	Set_CRAM_1K_Bank(1, bank * 8 + 1);
	Set_CRAM_1K_Bank(2, bank * 8 + 2);
	Set_CRAM_1K_Bank(3, bank * 8 + 3);
	Set_CRAM_1K_Bank(4, bank * 8 + 4);
	Set_CRAM_1K_Bank(5, bank * 8 + 5);
	Set_CRAM_1K_Bank(6, bank * 8 + 6);
	Set_CRAM_1K_Bank(7, bank * 8 + 7);
}