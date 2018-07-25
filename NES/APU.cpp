#include "../stdafx.h"
#include "APU.h"
#include "NES.h"

#define SUBCNT(var,n) { if (var>n) {var -= n;} else {var = 0;} }

APU::APU(NES* p) {
	nes = p;

	memset(CNT, 0, sizeof(CYCEL));
	memset(CNT, 0, sizeof(CNT));

	sound_cycel = nes->nescfg->CpuClock / (FLOAT)(nes->nescfg->FrameRate) / 882.0f;
	sound_cycel = nes->nescfg->CpuClock  / 44100;
	quee_index = 0;
}

void APU::Sync_Cycel(INT cycel) {
	//SUBCNT(CNT[0].Timer, cycel); // ����������, Ƶ�ʸ�CPUƵ��һ�������Լ���CPU���е�ʵ������
	//SUBCNT(CNT[1].Timer, cycel); // ����������, Ƶ�ʸ�CPUƵ��һ�������Լ���CPU���е�ʵ������
	if (CNT[0].Timer > cycel) {
		CNT[0].Timer -= cycel;
	}
	else { // �������Զ����� ���Ƶ�ʷ�����ռ�����ڷ�����
		WORD tmp = cycel - CNT[0].Timer;
		CNT[0].Timer = (((WORD)(nes->REG[3] & 0x07) << 8) | nes->REG[2]) + 1;
		if (CNT[0].Timer > tmp)
			CNT[0].Timer -= tmp;

		if (CNT[0].Empty == 0) {
			CNT[0].Empty = 0x10;
		}
		CNT[0].Empty--;
		if (CNT[0].Empty == 0) {
			this->SetVolume(0);
		}
		this->SetSoundCycle(cycel, 0);
		this->SetQuee(cycel, 0);
	}
}

void APU::Write(WORD addr, BYTE value) {
	BYTE index = addr & 0x0F;
	switch (index)
	{
	case 0x00:
	case 0x04:
	case 0x0C:
		CNT[index>>2].Volume = nes->REG[index]&0x10 ? value&0x0F : (value&0x0F) << 8; // ��4λΪ����˥����
		if (index == 0  && value & 0x0F) {
			//::MessageBox(NULL, L"������", L"Notify", MB_OK);
		}
		switch ((value>>6)&0x03) { // ��6��7λΪռ����������
		case 0x00:
			CNT[index >> 2].Empty = 2;
			break;
		case 0x01:
			CNT[index >> 2].Empty = 4;
			break;
		case 0x02:
			CNT[index >> 2].Empty = 8;
			break;
		case 0x03:
			CNT[index >> 2].Empty = 12;
			break;
		default:
			break;
		}
		break;
	case 0x01:
		break;
	case 0x02:
		CNT[0].Timer = (((WORD)(nes->REG[3] & 0x07) << 8) | nes->REG[2]) + 1;
		CNT[0].Wave = CNT[0].Timer;
		break;
	case 0x03:
		CNT[0].Timer = (((WORD)(nes->REG[3] & 0x07) << 8) | nes->REG[2]) + 1;
		CNT[0].Wave = CNT[0].Timer;
		SetSoundLength(0, value);
		break;
	default:
		break;
	}
}

void APU::SetSoundLength(BYTE index, BYTE value) {
	if (!(nes->REG[0x15] & (1<<index))) { // ����ֹ ��Զ��0
		CNT[index].Sound = 0;
		::MessageBox(NULL, L"������ֹ", L"Notify", MB_OK);
		return;
	}
	if (!(value & 0x08)) { // ��3λ��0
		BYTE cd = !(value & 0x80);
		switch ((value >> 4) & 0x07) { // 4��6λ��ֵ
		case 0:
			// ��7λֵ��һ�� ��Ӧ��ֵҲ��һ��
			W_IF2(CNT[index].Sound, cd, 0x05, 0x06);
			break;
		case 1:
			W_IF2(CNT[index].Sound, cd, 0x0A, 0x0C);
			break;
		case 2:
			W_IF2(CNT[index].Sound, cd, 0x14, 0x18);
			break;
		case 3:
			W_IF2(CNT[index].Sound, cd, 0x28, 0x30);
			break;
		case 4:
			W_IF2(CNT[index].Sound, cd, 0x50, 0x60);
			break;
		case 5:
			W_IF2(CNT[index].Sound, cd, 0x1E, 0x24);
			break;
		case 6:
			W_IF2(CNT[index].Sound, cd, 0x07, 0x08);
			break;
		case 7:
			W_IF2(CNT[index].Sound, cd, 0x0E, 0x10);
			break;
		default:
			break;
		}
	}
	else { // ��3λ��1
		// 4��7λ��ֵ
		W_IF2(CNT[index].Sound, (value>>4)&0x0F, (value>>4)&0x0F, 0x7F);
	}
}

void APU::SetVolume(BYTE index) {
	BYTE reg_index = index * 4;
	if (nes->REG[reg_index] & 0x10) { // ��4λΪ1��˥��
		CNT[index].Volume = nes->REG[reg_index] & 0x0F;
	}
	else {
		if (CNT[index].Volume == 0) {
			if (nes->REG[reg_index] & 0x20) { // ����˥��ѭ�� ��5λ1Ϊ����
				CNT[index].Volume = 0x0F << 8;
				::MessageBox(NULL, L"��ѭ��˥����", L"Notify", MB_OK);
			}
		}
		else {
			BYTE rate = 240 / ((nes->REG[reg_index] & 0x0F) + 1); // ˥����
			CString fuck;
			fuck.Format(L"Volume:%d, RATE:%d", CNT[index].Volume, rate);
			//::MessageBox(NULL, fuck, L"Notify", MB_OK);
			if (CNT[index].Volume < rate)
				CNT[index].Volume = 0;
			else
				CNT[index].Volume -= rate;
		}
	}
}


void APU::SetSoundCycle(INT cycel, BYTE index) {
	// ������������60HZ
	INT frame_cycels = nes->nescfg->CpuClock / 60;
	CYCEL.Sound += cycel;
	if (CYCEL.Sound >= frame_cycels) {
		CYCEL.Sound -= frame_cycels;
		// ����λ�����λΪ1�ǽ�ֹ
		BYTE forbid = 0;
		W_IF2(forbid, index!=2, nes->REG[index*4]&0x20, nes->REG[2*4]&0x80);
		if (!forbid && CNT[index].Sound > 0) { // û�н�ֹ����
			CNT[index].Sound--; // ����������-1
		}
	}
}

void APU::SetQuee(INT cycel, BYTE index) {
	CYCEL.Quee += (FLOAT)cycel;
	//CString fuck;
	//fuck.Format(L"Quee:%.2f", CYCEL.Quee);
	//::MessageBox(NULL, fuck, L"Notify", MB_OK);
	if (CYCEL.Quee >= sound_cycel) {
		if (CNT[index].Sound > 0) {
			sound_data[quee_index] = CNT[index].Volume;
			//CString fuck;
			//fuck.Format(L"Volume:%d", CNT[index].Volume);
			//::MessageBox(NULL, fuck, L"Notify", MB_OK);
		}
		else {
			sound_data[quee_index] = 0;
		}
		if (++quee_index >= 8192)
			quee_index = 0;

		CYCEL.Quee -= sound_cycel;

		//CString fuck;
		//fuck.Format(L"Quee:%.2f", CYCEL.Quee);
		//::MessageBox(NULL, fuck, L"Notify", MB_OK);
	}
}
