#include "Mapper003.h"

void Mapper003::RESET() {
	Set_PRGRom_32K_Bank(0, 1, PRGRom_8K_Size - 2, PRGRom_8K_Size - 1);
	Set_CHRRom_8K_Bank(0);
}

void Mapper003::Write(WORD addr, BYTE data) {
	CString d;
	d.Format(L"CHR BANK:%d, %d", data, data & 0x03);
	//::MessageBox(NULL, d, L"", MB_OK);
	Set_CHRRom_8K_Bank(data & 0x03);
}