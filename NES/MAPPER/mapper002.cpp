#include "Mapper002.h"

void Mapper002::RESET() {
	Set_PRGRom_32K_Bank(0, 1, PRGRom_8K_Size - 2, PRGRom_8K_Size - 1);
	Set_CRAM_8K_Bank(0);
}

void Mapper002::Write(WORD addr, BYTE data) {
	Set_PRGRom_16K_Bank(4, data);
}