#include "mapper2.h"

void Mapper2::REST() {
	Set_PRGRom_32K_Bank(0, 1, PRGRom_8K_Size - 2, PRGRom_8K_Size - 1);
}

void Mapper2::Write(WORD addr, BYTE data) {
	Set_PRGRom_16K_Bank(4, data);
}