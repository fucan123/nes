#pragma once

class NES;

class APU {
public:
	NES* nes;

	// ������
	WORD CNT[4][2];
public:
	APU(NES*);

	void Write(WORD addr, BYTE value);
	void Sync_Cycel(INT cycel);
};