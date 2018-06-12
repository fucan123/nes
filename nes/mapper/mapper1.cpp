#include "MAPPER1.h"

void Mapper1::RESET() {
	Set_PRGRom_32K_Bank(0, 1, PRGRom_8K_Size - 2, PRGRom_8K_Size - 1);
}