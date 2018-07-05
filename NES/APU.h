#pragma once

class NES;

class APU {
public:
	NES* nes;

	// ¼ÆÊýÆ÷
	WORD CNT[4][2];
public:
	APU(NES*);

	void Write(WORD addr, BYTE value);
	void Sync_Cycel(INT cycel);
};