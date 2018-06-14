#pragma once

#include "../stdafx.h"

extern BYTE* CPU_MEM_BANK[8];  // 8K段, CPU最大支持内存0x1000[0x0000-0xFFFF], 所以需要8个储存单元(0x10000/0x2000=8)
extern BYTE RAM [8 * 1024];    // 2K-RAM 做了4次镜像, 0x0000-0x1FFF
extern BYTE PRAM[8 * 1024];    // PPU寄存器0x2000-0x2007, 其他是镜像, 0x2000-0x3FFF
extern BYTE ERAM[8 * 1024];    // 0x4000-0x401F为寄存器, 0x4020-0x5FFF为扩展ROM, 0x4000-0x5FFFF
extern BYTE SRAM[8 * 1024];    // 卡带的SRAM, 0x6000-0x7FFF

extern BYTE* PPU_MEM_BANK[12]; // PPU 1K段
extern BYTE  CRAM[32 * 1024];  // PPU 内存 32KB
extern BYTE  BGPAL[0x10];      // 背景调色板
extern BYTE  SPPAL[0x10];      // 精灵调色板

extern BYTE* PRGRom;           // 程序ROM地址
extern BYTE* CHRRom;           // 角色ROM地址

extern BYTE PRGRom_8K_Size;    // PRGRom 数量
extern BYTE CHRRom_1K_Size;    // CHRRom 1K 数量

void MMC_INIT();

void Set_PRGRom_Bank(BYTE page, BYTE* ptr);
void Set_PRGRom_8K_Bank(BYTE page, BYTE bank);
void Set_PRGRom_16K_Bank(BYTE page, BYTE bank);
void Set_PRGRom_32K_Bank(BYTE bank);
void Set_PRGRom_32K_Bank(BYTE bank0, BYTE bank1, BYTE bank2, BYTE bank3);

void Set_CHRRom_Bank(BYTE page, BYTE* ptr);
void Set_CHRRom_1K_Bank(BYTE page, BYTE bank);
void Set_CHRRom_2K_Bank(BYTE page, BYTE bank);
void Set_CHRRom_4K_Bank(BYTE page, BYTE bank);
void Set_CHRRom_8K_Bank(BYTE bank);
void Set_CHRRom_8K_Bank(BYTE bank0, BYTE bank1, BYTE bank2, BYTE bank3, BYTE bank4, BYTE bank5, BYTE bank6, BYTE bank7);

void Set_CRAM_Bank(BYTE page, BYTE* ptr);
void Set_CRAm_1K_Bank(BYTE page, BYTE bank);
void Set_CRAM_2K_Bank(BYTE page, BYTE bank);
void Set_CRAM_4K_Bank(BYTE page, BYTE bank);
void Set_CRAM_8K_Bank(BYTE bank);