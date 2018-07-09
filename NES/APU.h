#pragma once

typedef struct S_APU_CYCYE {
	INT Sound; // �������ڼ���
} APU_CYCEL;

typedef struct S_APU_CNT {
	WORD Timer; // ʮһλ�ɱ�̲�����ʱ��
	BYTE Sound; // ����������
	WORD Wave;  // ����
} APU_CNT;

class NES;

class APU {
public:
	NES* nes;

	// ���ڼ���
	APU_CYCEL CYCEL;
	// ������
	APU_CNT CNT[4];
public:
	APU(NES*);

	void Write(WORD addr, BYTE value);
	void SetSoundLength(BYTE index, BYTE value);
	void Sync_Cycel(INT cycel);
	void SetSoundCycle(INT cycel, BYTE index);
};