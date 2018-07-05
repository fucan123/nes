#include "../stdafx.h"
#include "APU.h"
#include "NES.h"

#define SUBCNT(p,n) { if (*p > n){ *p -= n; } else { *p = 0; } }

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
		CNT[0][0] = (((WORD)(nes->REG[3] & 0x07) << 8) | nes->REG[2]) + 1;
		break;
	case 0x03:
		CNT[0][0] = (((WORD)(nes->REG[3] & 0x07) << 8) | nes->REG[2]) + 1;
		break;
	default:
		break;
	}
}

void APU::Sync_Cycel(int cycel) {
	SUBCNT(&CNT[0][0], cycel); // 计数器减少, 频率跟CPU频率一样，所以减少CPU运行的实际周期
	SUBCNT(&CNT[1][0], cycel); // 计数器减少, 频率跟CPU频率一样，所以减少CPU运行的实际周期
}