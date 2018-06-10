#include "rom.h"

ROM::ROM() {
	file = NULL;
	memset(&header, 0, sizeof(NES_HEADER));
}

bool ROM::open(char* file) {
	FILE* fp = fopen(file, "r");
	if (!fp) {
		return false;
	}

	this->file = file;
	struct stat st;
	fstat(_fileno(fp), &st);
	_off_t size = st.st_size; //文件大小
	char* rom = new char[size];
	if (!rom) {
		return false;
	}
	memset(rom, 0, size);
	fread(rom, size, 1, fp);
	memcpy(&header, rom, sizeof(NES_HEADER));

	return true;
}