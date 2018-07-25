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
	//SUBCNT(CNT[0].Timer, cycel); // 计数器减少, 频率跟CPU频率一样，所以减少CPU运行的实际周期
	//SUBCNT(CNT[1].Timer, cycel); // 计数器减少, 频率跟CPU频率一样，所以减少CPU运行的实际周期
	if (CNT[0].Timer > cycel) {
		CNT[0].Timer -= cycel;
	}
	else { // 计算器自动重载 输出频率反馈到占空周期发生器
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
		CNT[index>>2].Volume = nes->REG[index]&0x10 ? value&0x0F : (value&0x0F) << 8; // 低4位为音量衰减率
		if (index == 0  && value & 0x0F) {
			//::MessageBox(NULL, L"有音量", L"Notify", MB_OK);
		}
		switch ((value>>6)&0x03) { // 第6，7位为占空周期类型
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
	if (!(nes->REG[0x15] & (1<<index))) { // 被禁止 永远是0
		CNT[index].Sound = 0;
		::MessageBox(NULL, L"音长禁止", L"Notify", MB_OK);
		return;
	}
	if (!(value & 0x08)) { // 第3位＝0
		BYTE cd = !(value & 0x80);
		switch ((value >> 4) & 0x07) { // 4－6位的值
		case 0:
			// 第7位值不一样 对应的值也不一样
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
	else { // 第3位＝1
		// 4－7位的值
		W_IF2(CNT[index].Sound, (value>>4)&0x0F, (value>>4)&0x0F, 0x7F);
	}
}

void APU::SetVolume(BYTE index) {
	BYTE reg_index = index * 4;
	if (nes->REG[reg_index] & 0x10) { // 第4位为1无衰减
		CNT[index].Volume = nes->REG[reg_index] & 0x0F;
	}
	else {
		if (CNT[index].Volume == 0) {
			if (nes->REG[reg_index] & 0x20) { // 不断衰减循环 第5位1为允许
				CNT[index].Volume = 0x0F << 8;
				::MessageBox(NULL, L"不循环衰减！", L"Notify", MB_OK);
			}
		}
		else {
			BYTE rate = 240 / ((nes->REG[reg_index] & 0x0F) + 1); // 衰减率
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
	// 音长计数器是60HZ
	INT frame_cycels = nes->nescfg->CpuClock / 60;
	CYCEL.Sound += cycel;
	if (CYCEL.Sound >= frame_cycels) {
		CYCEL.Sound -= frame_cycels;
		// 第五位或最高位为1是禁止
		BYTE forbid = 0;
		W_IF2(forbid, index!=2, nes->REG[index*4]&0x20, nes->REG[2*4]&0x80);
		if (!forbid && CNT[index].Sound > 0) { // 没有禁止计数
			CNT[index].Sound--; // 音长计数器-1
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
