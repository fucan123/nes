#include "Mapper000.h"

void Mapper000::RESET() {
	Set_PRGRom_32K_Bank(0, 1, PRGRom_8K_Size - 2, PRGRom_8K_Size - 1);
	Set_CHRRom_8K_Bank(0);
}