#include "MMU.h"

BYTE* CPU_MEM_BANK[8];  // 8K段, CPU最大支持内存0x10000[0x0000-0xFFFF], 所以需要8个储存单元(0x10000/0x2000=8)
BYTE  RAM [8 * 1024];   // 2K-RAM 做了4次镜像, 0x0000-0x1FFF
BYTE  PRAM[8 * 1024];   // PPU寄存器0x2000-0x2007, 其他是镜像, 0x2000-0x3FFF
BYTE  ERAM[8 * 1024];   // 0x4000-0x401F为寄存器, 0x4020-0x5FFF为扩展ROM, 0x4000-0x5FFFF
BYTE  SRAM[8 * 1024];   // 卡带的SRAM, 0x6000-0x7FFF

BYTE* PRGRom;           // 程序ROM地址
BYTE* CHRRom;           // 角色ROM地址

BYTE PRGRom_8K_Size;    // PRGRom 数量

void MMC_INIT() {
	Set_PRGRom_Bank(0, RAM);
	Set_PRGRom_Bank(1, PRAM);
	Set_PRGRom_Bank(2, ERAM);
	Set_PRGRom_Bank(3, SRAM);
}

void Set_PRGRom_Bank(BYTE page, BYTE* ptr) {
	CPU_MEM_BANK[page] = ptr;
}

void Set_PRGRom_8K_Bank(BYTE page, BYTE bank) {
	CPU_MEM_BANK[page] = PRGRom + (bank * 0x2000); //每个BANK 8K=0x2000
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