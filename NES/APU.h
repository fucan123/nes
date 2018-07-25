#pragma once

typedef struct S_APU_CYCYE {
	INT Sound; // 音长周期计数
	FLOAT Quee;  // 声音数据
} APU_CYCEL;

typedef struct S_APU_CNT {
	WORD Timer;  // 十一位可编程波长定时器
	BYTE Sound;  // 音长计数器
	WORD Wave;   // 波长
	WORD Volume; // 音量
	BYTE Empty;  // 占空周期
} APU_CNT;

class NES;

class APU {
public:
	NES* nes;

	FLOAT sound_cycel;

	INT quee_index;

	// 周期计数
	APU_CYCEL CYCEL;

	// 计数器
	APU_CNT CNT[4];

	// 声音数据
	WORD sound_data[2 * 8192];
public:
	APU(NES*);

	void Sync_Cycel(INT cycel);
	void Write(WORD addr, BYTE value);
	void SetSoundLength(BYTE index, BYTE value);
	void SetVolume(BYTE index);
	void SetSoundCycle(INT cycel, BYTE index);
	void SetQuee(INT cycel, BYTE index);
};