#pragma once

#ifndef PPUH
#define PPUH

class PPU {
public:
	//寄存器
	byte REG[8];

	//所有PPU内存
	byte  MEM[0x7fff + 1];

	//背景图形地址
	byte* BGA;

	//精灵图形地址
	byte* SPRA;

	//命名表0
	byte* N_TABLE;

	//属性表0
	byte* A_TABLE;

	//背景调色板地址
	byte* BGC_TABLE;

	//字模对应的属性编号[每画面有960个字幕]
	byte  CAP_TBALE[960];

	CPaintDC* dc;
public:
	PPU();
	//装载卡带图形数据
	void load(char*, size_t);
	//读取寄存器
	byte readREG(byte addr);
	//写入寄存器
	void writeREG(byte addr, byte value);
	//绘制背景
	void getBG(byte images[]);
	//绘制一条扫描线(分辨率256*240，一共240条) line=要绘制的线编号
	void inline scanfLine(byte line, byte images[]);
	int inline rgb(byte);
};

#endif // !PPUH
