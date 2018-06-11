#include "ROM.h"

ROM::ROM() {
	file = NULL;
	rom = NULL;
	memset(&header, 0, sizeof(NES_HEADER));
}

bool ROM::Open(char* file) {
	FILE* fp = fopen(file, "r");
	if (!fp) {
		return false;
	}

	this->file = file;
	struct stat st;
	fstat(_fileno(fp), &st);
	_off_t size = st.st_size; //文件大小
	rom = new BYTE[size];
	if (!rom) {
		return false;
	}
	memset(rom, 0, size);
	fread(rom, size, 1, fp);
	memcpy(&header, rom, sizeof(NES_HEADER));

	PRGRom = rom + sizeof(NES_HEADER);
	CHRRom = PRGRom + header.PRG_PAGE_SIZE * 0x4000; // PRGRom 每页16KB
	PRGRom_8K_Size = header.PRG_PAGE_SIZE * 2;

	return true;
}