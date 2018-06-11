#pragma once

#include "../stdafx.h"

extern BYTE* CPU_MEM_BANK[8]; // 8K段, CPU最大支持内存0x1000[0x0000-0xFFFF], 所以需要8个储存单元(0x10000/0x2000=8)
extern BYTE RAM [8 * 1024];   // 2K-RAM 做了4次镜像, 0x0000-0x1FFF
extern BYTE PRAM[8 * 1024];   // PPU寄存器0x2000-0x2007, 其他是镜像, 0x2000-0x3FFF
extern BYTE ERAM[8 * 1024];   // 0x4000-0x401F为寄存器, 0x4020-0x5FFF为扩展ROM, 0x4000-0x5FFFF
extern BYTE SRAM[8 * 1024];   // 卡带的SRAM, 0x6000-0x7FFF

extern BYTE* PRGRom;          // 程序ROM地址
extern BYTE* CHRRom;          // 角色ROM地址

extern BYTE PRGRom_8K_Size;  // PRGRom 数量

void MMC_INIT();
void Set_PRGRom_Bank(BYTE page, BYTE* ptr);
void Set_PRGRom_8K_Bank(BYTE page, BYTE bank);
void Set_PRGRom_16K_Bank(BYTE page, BYTE bank);
void Set_PRGRom_32K_Bank(BYTE bank);
void Set_PRGRom_32K_Bank(BYTE bank0, BYTE bank1, BYTE bank2, BYTE bank3);