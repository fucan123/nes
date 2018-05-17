#pragma once
#include "stdafx.h"
#include "ppu.h"

#define VBLANK_FLAG 0x80
#define SET_VBLANK()   REG[2] |= VBLANK_FLAG
#define CLEAR_VBLANK() REG[2] &= (~VBLANK_FLAG)

PPU::PPU() {
	BGA     = MEM + 0x1000;
	SPRA    = MEM + 0x0000;
	N_TABLE = MEM + 0x2000;
	A_TABLE = MEM + 0x23C0;
	BGC_TABLE = MEM + 0x3F00;
	for (int i = 0; i < 960; i++) {
		byte line = i >> 5; //除以32 每行32个字幕
		byte n = (i - (line * 32)) >> 2; //每行隔4个字幕增加一
		n += (line >> 2) * 8; //隔4列+8 因为每行8个字幕
		CAP_TBALE[i] = n; //对应属性表的编号
		//第一二行前两个字幕(2*2)占0-1位 第一二行后两个字幕占2-3位 第三四行前两个字幕(2*2)占4-5位 第三四行后两个字幕占6-7位
		//把上面编号依次编号0,1,2,3
		byte bit = ((i - (line * 32)) & 0x03) >> 1; //决定一行中两组占用的编号(0-1)
		if ((line & 0x03) > 1) { //到了第三四行
			bit += 2; //第一二行为0,1 第三四行就为2,3
		}
		//因为n最多是63 高位2位用不到 用于保存此表项对应的属性位编号(0-3)
		CAP_TBALE[i] |= (bit << 6);
		//0-3(0), 4-7(2) 33-36(0)
	}
	dc = NULL;
}

void PPU::load(char* m, size_t size) {
	memset(MEM, 0, 0x8000);
	memcpy(MEM, m, size);
}
//读取寄存器
byte PPU::readREG(byte addr) {
	return REG[addr & 0x07];
}
//写入寄存器
void PPU::writeREG(byte addr, byte value) {
	REG[addr & 0x07] = value;
}

void PPU::showBG() {
	//逐行绘制(分辨率256*240，240行)
	for (int i = 0; i < 240; i++) {
		this->scanfLine(i);
	}
	SET_VBLANK();
	/*
	CPen pen(PS_SOLID, 1, RGB(0, 0, 0));//创建一个虚线线条，宽度为1，红色的画笔对象  
	CPen* pOldPen = dc->SelectObject(&pen);//将画笔对象选入到设备描述表中 
	int tl = 60;
	POINT ps = { 0, tl };
	POINT pe = { 256, tl };
	dc->MoveTo(ps);
	dc->LineTo(pe);
	dc->SelectObject(pOldPen);
	pen.DeleteObject();
	*/
}

void PPU::scanfLine(byte line) {
	//属于画面中哪个画面(属于命名表中几号编号) 一共8行 每行32个
	word n = (line >> 3) * 32;
	//字模中开始地址 占2个字节 每个字模16字节
	word m = (line & 0x07) * 2;
	//一行32个字幕 sx=x开始坐标
	word e = n + 32, sx = 0;
	if (line == 60) {
		CString t;
		t.Format(L"n=%d,m=%d,e=%d,line>>3=%d", n, m, e, line >> 3);
		//MessageBox(NULL, t, L"t", MB_OK);
	}
	while (n < e) {
		//字模编号
		byte tn = N_TABLE[n];
		//字幕起始地址
		byte* addr = BGA + (tn * 16);
		//此字模属性 低6位是属性编号
		byte attr = A_TABLE[CAP_TBALE[n] & 0x3f];
		//颜色组
		byte group = CAP_TBALE[tn] >> 6;
		//背景颜色调色板地址 4组 每组4字节 共16字节
		byte* col_addr = BGC_TABLE + (group * 4);
		word title = *((word*)(addr + m));
		//每两位表示一个像素在颜色组中的位置
		for (byte i = 0; i < 16; i += 2) {
			//在调色板组中的位置
			byte pos = (title >> i) & 0x03;
			//获取颜色
			byte color = *(col_addr + pos);
			if (dc) {
				CPen pen(PS_SOLID, 1, this->rgb(color) + line);//创建一个虚线线条，宽度为1，红色的画笔对象  
				CPen* pOldPen = dc->SelectObject(&pen);//将画笔对象选入到设备描述表中 
				POINT ps = {sx, line};
				POINT pe = {sx + 1, line};
				dc->MoveTo(ps);
				dc->LineTo(pe);
				dc->SelectObject(pOldPen);
				pen.DeleteObject();
				if (line == 60) {
					//MessageBox(NULL, L"60", L"t", MB_OK);
				}
			}
			sx += 1;
		}
		n++;
	}
}
//调色板共支持64种颜色
int PPU::rgb(byte n) {
	switch (n)
	{
	case 0x00:
		return RGB(0x80, 0x80, 0x80);
	case 0x01:
		return RGB(0x00, 0x3D, 0xA6);
	case 0x02:
		return RGB(0x00, 0x12, 0xB0);
	case 0x03:
		return RGB(0x44, 0x00, 0x96);
	case 0x04:
		return RGB(0xA1, 0x80, 0x51);
	case 0x05:
		return RGB(0xC7, 0x00, 0x28);
	case 0x06:
		return RGB(0xBA, 0x06, 0x00);
	case 0x07:
		return RGB(0x8C, 0x17, 0x00);
	case 0x08:
		return RGB(0x5C, 0x2F, 0x00);
	case 0x09:
		return RGB(0x10, 0x45, 0x00);
	case 0x0A:
		return RGB(0x05, 0x4A, 0x00);
	case 0x0B:
		return RGB(0x00, 0x47, 0x2E);
	case 0x0C:
		return RGB(0x00, 0x41, 0x66);
	case 0x0D:
		return RGB(0x00, 0x00, 0x00);
	case 0x0E:
		return RGB(0x05, 0x05, 0x05);
	case 0x0F:
		return RGB(0x08, 0x08, 0x08);
	case 0x10:
		return RGB(0xC7, 0xC7, 0xC7);
	case 0x11:
		return RGB(0x00, 0x77, 0xFF);
	case 0x12:
		return RGB(0x21, 0x55, 0xFF);
	case 0x13:
		return RGB(0x82, 0x37, 0xFA);
	case 0x14:
		return RGB(0xEB, 0x2F, 0xB5);
	case 0x15:
		return RGB(0xFF, 0x29, 0x50);
	case 0x16:
		return RGB(0xFF, 0x22, 0x00);
	case 0x17:
		return RGB(0xD6, 0x32, 0x00);
	case 0x18:
		return RGB(0xC4, 0x62, 0x00);
	case 0x19:
		return RGB(0x35, 0x80, 0x00);
	case 0x1A:
		return RGB(0x05, 0x8F, 0x80);
	case 0x1B:
		return RGB(0x00, 0x8A, 0x55);
	case 0x1C:
		return RGB(0x00, 0x99, 0xCC);
	case 0x1D:
		return RGB(0x21, 0x21, 0x21);
	case 0x1E:
		return RGB(0x09, 0x09, 0x09);
	case 0x1F:
		return RGB(0x07, 0x07, 0x07);
	case 0x20:
		return RGB(0xFF, 0xFF, 0xFF);
	case 0x21:
		return RGB(0x0F, 0xD7, 0xFF);
	case 0x22:
		return RGB(0x69, 0xA2, 0xFF);
	case 0x23:
		return RGB(0xD4, 0x80, 0xFF);
	case 0x24:
		return RGB(0xFF, 0x45, 0xF3);
	case 0x25:
		return RGB(0xFF, 0x61, 0x8B);
	case 0x26:
		return RGB(0xFF, 0x88, 0x33);
	case 0x27:
		return RGB(0xFF, 0x9C, 0x12);
	case 0x28:
		return RGB(0xFA, 0xBC, 0x20);
	case 0x29:
		return RGB(0x9f, 0xE3, 0x0E);
	case 0x2A:
		return RGB(0x2B, 0xF0, 0x35);
	case 0x2B:
		return RGB(0x0C, 0xF0, 0xA4);
	case 0x2C:
		return RGB(0x05, 0xFB, 0xFF);
	case 0x2D:
		return RGB(0x5E, 0x5E, 0x5E);
	case 0x2E:
		return RGB(0x0D, 0x0D, 0x0D);
	case 0x2F:
		return RGB(0x0C, 0x0C, 0x0C);
	case 0x30:
		return RGB(0xFF, 0xFF, 0xFF);
	case 0x31:
		return RGB(0xA6, 0xFC, 0xFF);
	case 0x32:
		return RGB(0xB3, 0xEC, 0xFF);
	case 0x33:
		return RGB(0xDA, 0xAB, 0xEB);
	case 0x34:
		return RGB(0xFF, 0xA8, 0xF9);
	case 0x35:
		return RGB(0xFF, 0xAB, 0xB3);
	case 0x36:
		return RGB(0xFF, 0xD2, 0xB0);
	case 0x37:
		return RGB(0xFF, 0xEF, 0xA6);
	case 0x38:
		return RGB(0xFF, 0xF7, 0x9C);
	case 0x39:
		return RGB(0xD7, 0xE8, 0x95);
	case 0x3A:
		return RGB(0xA6, 0xED, 0xAF);
	case 0x3B:
		return RGB(0xA2, 0xF2, 0xDA);
	case 0x3C:
		return RGB(0x99, 0xFF, 0xFC);
	case 0x3D:
		return RGB(0xDD, 0xDD, 0xDD);
	case 0x3E:
		return RGB(0x11, 0x11, 0x11);
	case 0x3F:
		return RGB(0x22, 0x22, 0x22);
	default:
		return RGB(0x0, 0x0, 0x0);
	}
}