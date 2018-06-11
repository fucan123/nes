#pragma once

#include "../stdafx.h"

extern BYTE* CPU_MEM_BANK[8]; // 8K��, CPU���֧���ڴ�0x1000[0x0000-0xFFFF], ������Ҫ8�����浥Ԫ(0x10000/0x2000=8)
extern BYTE RAM [8 * 1024];   // 2K-RAM ����4�ξ���, 0x0000-0x1FFF
extern BYTE PRAM[8 * 1024];   // PPU�Ĵ���0x2000-0x2007, �����Ǿ���, 0x2000-0x3FFF
extern BYTE ERAM[8 * 1024];   // 0x4000-0x401FΪ�Ĵ���, 0x4020-0x5FFFΪ��չROM, 0x4000-0x5FFFF
extern BYTE SRAM[8 * 1024];   // ������SRAM, 0x6000-0x7FFF

extern BYTE* PRGRom;          // ����ROM��ַ
extern BYTE* CHRRom;          // ��ɫROM��ַ

extern BYTE PRGRom_8K_Size;  // PRGRom ����

void MMC_INIT();
void Set_PRGRom_Bank(BYTE page, BYTE* ptr);
void Set_PRGRom_8K_Bank(BYTE page, BYTE bank);
void Set_PRGRom_16K_Bank(BYTE page, BYTE bank);
void Set_PRGRom_32K_Bank(BYTE bank);
void Set_PRGRom_32K_Bank(BYTE bank0, BYTE bank1, BYTE bank2, BYTE bank3);