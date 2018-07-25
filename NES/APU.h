#pragma once

typedef struct S_APU_CYCYE {
	INT Sound; // �������ڼ���
	FLOAT Quee;  // ��������
} APU_CYCEL;

typedef struct S_APU_CNT {
	WORD Timer;  // ʮһλ�ɱ�̲�����ʱ��
	BYTE Sound;  // ����������
	WORD Wave;   // ����
	WORD Volume; // ����
	BYTE Empty;  // ռ������
} APU_CNT;

class NES;

class APU {
public:
	NES* nes;

	FLOAT sound_cycel;

	INT quee_index;

	// ���ڼ���
	APU_CYCEL CYCEL;

	// ������
	APU_CNT CNT[4];

	// ��������
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