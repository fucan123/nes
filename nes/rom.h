#pragma once

#include "../stdafx.h"
#include <stdio.h>
#include <iostream>
#include "MMU.h"

typedef struct {
	BYTE ID[4];       //
	BYTE PRG_PAGE_SIZE;   //
	BYTE CHR_PAGE_Size;  //
	BYTE Control1;    //
	BYTE Control2;    //
	BYTE Ceserved[8]; //
} NES_HEADER;

class ROM {
public:
	char* file;
	BYTE* rom;
	NES_HEADER header;
public:
	ROM();
	~ROM() { if (rom) delete[] rom; }
	
	bool Open(char* file);
	char* GetFile() const { return file;  }
	NES_HEADER* GetHeader() { return &header;  }
	BYTE GetMapperNo() { return (header.Control1 >> 4) | (header.Control2 & 0xf0); }
};