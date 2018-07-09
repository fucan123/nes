#pragma once

typedef struct S_APU_CYCYE {
	INT Sound; // 音长周期计数
} APU_CYCEL;

typedef struct S_APU_CNT {
	WORD Timer; // 十一位可编程波长定时器
	BYTE Sound; // 音长计数器
	WORD Wave;  // 波长
} APU_CNT;

class NES;

class APU {
public:
	NES* nes;

	// 周期计数
	APU_CYCEL CYCEL;
	// 计数器
	APU_CNT CNT[4];
public:
	APU(NES*);

	void Write(WORD addr, BYTE value);
	void SetSoundLength(BYTE index, BYTE value);
	void Sync_Cycel(INT cycel);
	void SetSoundCycle(INT cycel, BYTE index);
};