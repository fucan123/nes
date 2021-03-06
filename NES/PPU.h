#pragma once

#ifndef PPUH
#define PPUH

class NES;

class PPU {
public:
	NES* nes;
	//寄存器6号地址
	word REG6_ADDR;
	word REG7_ADDR;
	word REG7_INC;
public:
	//寄存器
	byte REG[8];

	//寄存器标志
	byte REG_FLAG[8];

	byte SCROLL_REG[2];

	//所有PPU内存
	byte  MEM[0x8000];

	//精灵[角色]内存
	byte SRAM[0xff + 1];

	//手柄
	byte HAND[2];

	//手柄计数器
	byte HAND_COUNT[2];

	/*
	手柄按键信息1-按下
	1-A 2-B 3-选择 4-开始 5-上 6-下 7-左 8-右
	*/
	byte HAND_KEY[2][8];

	//背景图形地址
	byte* BGA;

	//精灵图形地址
	byte* SPRA;

	//命名表0
	byte* N_TABLE[4];

	//属性表0
	byte* A_TABLE[4];

	byte N_TABLE_INDEX;

	byte N_TABLE_V;

	//背景调色板地址
	byte* BGC_TABLE;
	//精灵调色板地址
	byte* SPR_TABLE;

	//字模对应的属性编号[每画面有960个字幕]
	byte  CAP_TBALE[960];

	byte  SPR_SIZE;

	bool IS_SET_REG6;
	bool IS_REG7_FIRST;
	//是否可响应NMI中断
	bool IS_NMI;
public:
	PPU(NES*);
	//装载卡带图形数据
	void load(BYTE*, size_t, byte);
	//读取寄存器
	byte readREG(byte addr);
	//写入寄存器
	void writeREG(byte addr, byte value);
	//读取内存
	BYTE Read(WORD addr);
	//写入内存
	void writeMEM(word addr, byte value);
	//写入数据到精灵内存
	void dmaSRAM(byte* src);
	// 设置镜像模式0-水平, 1-垂直
	void SetMirroring(byte);
	//绘制背景
	void getBG(byte images[]);
	//绘制一条扫描线(分辨率256*240，一共240条) line=要绘制的线编号
	void inline scanfLine(byte line, byte images[]);
	int inline rgb(byte);
};

#endif // !PPUH
