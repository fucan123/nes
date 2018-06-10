#pragma once
#include "../stdafx.h"
#include <stdio.h>
#include <iostream>

typedef struct {
	BYTE id[4];       //
	BYTE rom_count;   //
	BYTE vrom_count;  //
	BYTE control1;    //
	BYTE control2;    //
	BYTE reserved[8]; //
} NES_HEADER;

class ROM {
public:
	char* file;
	NES_HEADER header;
public:
	ROM();
	
	bool open(char* file);
	char* getFile() const { return file;  }
	NES_HEADER* getHeader() { return &header;  }
};