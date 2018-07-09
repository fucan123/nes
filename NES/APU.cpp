#include "../stdafx.h"
#include "APU.h"
#include "NES.h"

#define SUBCNT(var,n) { if (var>n) {var -= n;} else {var = 0;} }

APU::APU(NES* p) {
	nes = p;

	memset(CNT, 0, sizeof(CNT));
}

void APU::Write(WORD addr, BYTE value) {
	switch (addr & 0x0F)
	{
	case 0x00:
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

void APU::Sync_Cycel(int cycel) {
	SUBCNT(CNT[0].Timer, cycel); // 计数器减少, 频率跟CPU频率一样，所以减少CPU运行的实际周期
	SUBCNT(CNT[1].Timer, cycel); // 计数器减少, 频率跟CPU频率一样，所以减少CPU运行的实际周期
}

void APU::SetSoundCycle(INT cycel, BYTE index) {
	// 音长计数器是60HZ
	INT frame_cycels = nes->nescfg->CpuClock / 60;
	CYCEL.Sound += cycel;
	if (CYCEL.Sound >= frame_cycels) {
		CYCEL.Sound -= frame_cycels;
		// 第五位或最高位为1是禁止
		BYTE forbid = 0;
		W_IF2(forbid, index!=2, nes->REG[index*4]&0x20, nes->REG[0x08]&0x80);
		if (!forbid && CNT[index].Sound > 0) { // 没有禁止计数
			CNT[index].Sound--; // 音长计数器-1
		}
	}
}