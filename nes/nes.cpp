#include "NES.h"

void NES::Init() {
	try {
		/*ROM* rom = new ROM;
		if (!(rom = new ROM))
			throw "申请ROM内存失败！";
		if (!rom->Open("hdl-u.nes"))
			throw "游戏文件打开失败！";
		if (!(mapper = CreateMapper(rom->GetMapperNo()))) {
			char str[64];
			sprintf(str, "不支持的Mapper号(%d)！", rom->GetMapperNo());
			throw str;
		}

		MMC_INIT();
		mapper->REST();*/
	}
	catch (char* msg) {

	}
}