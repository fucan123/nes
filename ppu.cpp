#pragma once
#include "stdafx.h"
#include "ppu.h"

#define VBLANK_FLAG 0x80
#define SET_VBLANK() REG[2] |= VBLANK_FLAG
#define CLR_VBLANK() REG[2] &= (~VBLANK_FLAG)
#define SRAMIN(y, h, l) ((l >= y) && (l < (y + h)))

PPU::PPU() {
	BGA     = MEM + 0x1000;
	SPRA    = MEM + 0x0000;
	N_TABLE[0] = N_TABLE[1] = MEM + 0x2000;
	A_TABLE[0] = A_TABLE[1] = MEM + 0x23C0;
	BGC_TABLE = MEM + 0x3F00;
	SPR_TABLE = MEM + 0x3F10;
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
	SPR_SIZE = 8;
	REG7_INC = 0;
	IS_SET_REG6 = false;
	IS_REG7_FIRST = true;
	IS_NMI = false;
}

void PPU::load(char* m, size_t size) {
	memset(MEM, 0, 0x8000);
	memcpy(MEM, m, size);
	memset(SRAM, 0, 0xff + 1);
	memset(REG, 0, 8);
	memset(REG_FLAG, 0, 8);
	REG_FLAG[5] = 1;
	//memset(N_TABLE[0], 0x33, 1024);
	//memset(BGC_TABLE, 33, 16);
	REG6_ADDR = 0;
}
//读取寄存器
byte PPU::readREG(byte addr) {
	addr &= 0x07;
	//MessageBox(NULL, L"READ PPU REG!", L"t", MB_OK);
	byte value = REG[addr];
	switch (addr)
	{
	case 2: //读端口2002 vblank位复位
		REG_FLAG[5] = 0; //复位
		REG_FLAG[6] = 0; //复位
		REG6_ADDR = 0;
		//CLR_VBLANK();
		break;
	case 4: //读取精灵RAM
		value = SRAM[REG[3]]; //寄存器编号3是SRAM地址
		REG[3]++; //每次访问地址+1
		break;
	case 7: //读取VRAM内存 地址由6号寄存器设置
		if (!REG_FLAG[7]) {
			REG7_ADDR = REG6_ADDR;
			REG_FLAG[7] = 1;
		}
		value = MEM[REG7_ADDR]; //读取VRAM内存
		if (!IS_REG7_FIRST) { //第一次读的数据无效
			REG7_ADDR += (REG[0] & 0x04) ? 32 : 1; //第二位决定+32或+1
			if (IS_SET_REG6) {
				REG7_ADDR = REG6_ADDR;
				REG7_INC = 0;
				IS_SET_REG6 = false;
			}
		}
		else {
			value = 0xff;
			REG7_ADDR = REG6_ADDR;
			REG7_INC = 0;
			IS_REG7_FIRST = false;
			IS_SET_REG6 = false;
		}
		break;
	default:
		break;
	}
	return value;
}
//写入寄存器
void PPU::writeREG(byte addr, byte value) {
	addr &= 0x07;
	CString ts;
	ts.Format(L"REG地址:%X", REG);
	//MessageBox(NULL, ts, L"t", MB_OK);
	switch (addr)
	{
	case 4: //写精灵RAM
		SRAM[REG[3]] = value; //写入SRAM
		REG[3]++; //每次访问地址+1
		break;
	case 5: {
		SCROLL_REG[REG_FLAG[5]] = value;
		if (REG_FLAG[5] && value > 239) {
			SCROLL_REG[1] = 239;
		}
		if (value > 0) {
			CString ts;
			ts.Format(L"REG5 value:%x, FLAG:%d", value, REG_FLAG[5]);
			//MessageBox(NULL, ts, L"t", MB_OK);
		}
		REG_FLAG[5] ^= 1;
		break;
	}
	case 6: { //设置访问VRAM的地址 第一次高8位 第二次低8位
		if (!REG_FLAG[6]) { //写入高位
			REG6_ADDR = 0;
			REG6_ADDR |= (((word)value) << 8) & 0xff00;
			REG_FLAG[6] = 1;
			ts.Format(L"写入高位:%X, addr:%X, addr2:%X", value, REG6_ADDR, REG6_ADDR & 0xff00);
			if (value > 0x70) {
				CString ts;
				ts.Format(L"--, value:%x, addr:%x, addr2:%X", value, REG6_ADDR, REG6_ADDR & 0xff00);
				//MessageBox(NULL, ts, L"t", MB_OK);
			}
		}
		else { //写入低位
			REG6_ADDR |= (value & 0xff);
			REG_FLAG[6] = 0;
			ts.Format(L"写入低位:%X, addr:%X", value, REG6_ADDR);
		}
		//MessageBox(NULL, ts, L"t", MB_OK);
		if (REG6_ADDR > 0x8000) {
			CString ts;
			ts.Format(L"value:%x, addr:%x", value, REG6_ADDR);
			MessageBox(NULL, ts, L"t", MB_OK);
		}
		CString ts;
		ts.Format(L"REG6_ADDR:%X", REG6_ADDR);
		//MessageBox(NULL, ts, L"t", MB_OK);
		IS_SET_REG6 = true;
		break; }
	case 7: {
		if (REG6_ADDR > 0x8000) {
			//MessageBox(NULL, L"FUCK", L"t", MB_OK);
		}
		if (value > 0) {
			CString ts;
			ts.Format(L"value:%x, addr:%x", value, REG6_ADDR);
			//MessageBox(NULL, ts, L"t", MB_OK);
		}
		if (IS_SET_REG6) {
			REG7_ADDR = REG6_ADDR;
			REG_FLAG[7] = 2;
			REG7_INC = 0;
			IS_SET_REG6 = false;
		}
		this->writeMEM(REG7_ADDR, value); //写入VRAM内存
		if (REG6_ADDR + REG7_INC == 0x3F00) {
			CString ts;
			ts.Format(L"3F00");
			//MessageBox(NULL, ts, L"t", MB_OK);
		}
		if (REG6_ADDR + REG7_INC) {
			CString ts;
			ts.Format(L"2007 addr:%X, value：%X, REG7_INC:%X", REG6_ADDR + REG7_INC, value, REG7_INC);
			//MessageBox(NULL, ts, L"t", MB_OK);
		}
		REG7_ADDR += (REG[0] & 0x04) ? 32 : 1; //第二位决定+32或+1
		CString ts;
		ts.Format(L"2007 REG7_INC:%X, REG6 ADDR:%X", REG7_INC, REG6_ADDR);
		//MessageBox(NULL, ts, L"t", MB_OK);
		break; }
	default:
		REG[addr] = value;
		if (addr == 0) {
			N_TABLE_INDEX = value & 0x03;
			N_TABLE_V = value & 0x04 ? 1 : 0;
			if (N_TABLE_INDEX > 0 || N_TABLE_V > 0) {
				CString ts;
				ts.Format(L"index:%d, v:%d", N_TABLE_INDEX, N_TABLE_V);
				//MessageBox(NULL, ts, L"t", MB_OK);
			}
			
			if (value & 0x08) {
				BGA = MEM + 0x0000;
				SPRA = MEM + 0x1000;
			}
			if (value & 0x10) {
				BGA = MEM + 0x1000;
				SPRA = MEM + 0x0000;
			}
			if (value & 0x20) {
				SPR_SIZE = 16;
			}
			else {
				SPR_SIZE = 8;
			}
			if (N_TABLE_V) { //垂直镜像 
				N_TABLE[0] = N_TABLE[2] = &MEM[0x2000];
				N_TABLE[1] = N_TABLE[3] = &MEM[0x2400];
				A_TABLE[0] = A_TABLE[2] = &MEM[0x23C0];
				A_TABLE[1] = A_TABLE[3] = &MEM[0x27C0];
			}
			else { //水平镜像
				N_TABLE[0] = N_TABLE[1] = &MEM[0x2000];
				N_TABLE[2] = N_TABLE[3] = &MEM[0x2400];
				A_TABLE[0] = A_TABLE[1] = &MEM[0x23C0];
				A_TABLE[2] = A_TABLE[3] = &MEM[0x27C0];
			}
			//最高位控制是否响应NMI中断
			IS_NMI = value & 0x80 ? true : false;
		}
		break;
	}
}
//写入内存
void PPU::writeMEM(word addr, byte value) {
	if (addr >= 0x2800 && addr < 0x2C00) { //命名/属性表2(从0开始)地址
		word dim = addr - 0x2800;
		*(N_TABLE[2] + dim) = value;
		if (dim < 0x10) {
			CString ts;
			ts.Format(L"2800 addr:%X, value:%X, SCROLL_REG:%x,%x", addr, value, SCROLL_REG[0], SCROLL_REG[1]);
			//MessageBox(NULL, ts, L"t", MB_OK);
		}
		
	}
	else if (addr >= 0x2C00 && addr < 0x3000) { //命名/属性表3(从0开始)地址
		word dim = addr - 0x2C00;
		*(N_TABLE[3] + dim) = value;
	}
	else if (addr >= 0x3F00 && addr <= 0x3F0F) {
		MEM[addr] = value;
		//MEM[addr & 0x000F | 0x3F10] = value; //镜像到0x3F1?
	}
	else {
		MEM[addr] = value;
	}
}
//写入数据到精灵内存
void PPU::dmaSRAM(byte* src) {
	memcpy(SRAM, src, 0xff + 1);
}

void PPU::getBG(byte images[]) {
	//逐行绘制(分辨率256*240，240行)
	for (int i = 0; i < 240; i++) {
		this->scanfLine(i, images);
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

void PPU::scanfLine(byte line, byte images[]) {
	if (line == 0) {
		CLR_VBLANK();
	}
	byte nt_index = N_TABLE_INDEX;
	word line_t = line + SCROLL_REG[1];
	if (line_t > 239) {
		if (N_TABLE_INDEX == 0) {
			nt_index = 2;
		}
		else if (N_TABLE_INDEX == 1) {
			nt_index = 3;
		}
		else if (N_TABLE_INDEX == 2) {
			nt_index = 0;
		}
		else if (N_TABLE_INDEX == 3) {
			nt_index = 1;
		}
		line_t -= 240;
		CString t;
		t.Format(L"LINE T:%d, value:%X", line_t, N_TABLE[nt_index][100]);
		//MessageBox(NULL, t, L"t", MB_OK);
	}
	//属于画面中哪个画面(属于命名表中几号编号) 一共8行 每行32个
	word n = (line_t >> 3) * 32;
	//字模中开始地址 占2个字节 每个字模16字节
	word m = (line_t & 0x07);
	//一行32个字幕 sx=x开始坐标
	word e = n + 32, sx = 0;
	if (line_t == 60) {
		CString t;
		t.Format(L"n=%d,m=%d,e=%d,line>>3=%d", n, m, e, line >> 3);
		//MessageBox(NULL, t, L"t", MB_OK);
	}
	int index = line * 256 * 4;
	CString ts;
	ts.Format(L"n:%d, e:%d", n, e);
	//MessageBox(NULL, ts, L"t", MB_OK);
	//N_TABLE_INDEX = 0;
	while (n < e) {
		//字模编号
		byte tn = N_TABLE[nt_index][n];
		//字幕起始地址
		byte* addr = BGA + (tn * 16);
		//此字模属性 低6位是属性编号
		byte attr = A_TABLE[nt_index][CAP_TBALE[n] & 0x3f];
		//颜色组
		if ((attr)) {
			CString ts;
			ts.Format(L"attr:%0X", attr);
			//MessageBox(NULL, ts, L"t", MB_OK);
		}
		byte group = (attr >> ((CAP_TBALE[n] >> 6) << 1)) & 0x03;
		//group = CAP_TBALE[n] >> 6;
		//背景颜色调色板地址 4组 每组4字节 共16字节
		byte* col_addr = BGC_TABLE + (group * 4);
		//默认颜色
		byte def_color = *BGC_TABLE;
		//低位字节
		byte title_low = *(addr + m);
		//高位字节
		byte title_high = *(addr + m + 8);
		//每两位表示一个像素在颜色组中的位置 [前8位是低位，后8位是高位]
		for (int i = 7; i >= 0; i--) {
			//在调色板组中的位置
			byte pos  =  (title_low >> i)  & 0x01; //低位
			     pos |= ((title_high >> i) & 0x01) << 1; //高位
				 pos &= 0x3; //只有两位 
			//获取颜色 *(BGC_TABLE + pos) 
		    byte color_n = *BGC_TABLE;
			if (pos) {
				//if (pos == 0x1 || pos == 0x2) pos = 0x3;
				pos |= group << 2;
				color_n = *(BGC_TABLE + pos);
			}
			if (line == 239 && SCROLL_REG[1] > (8 * 10)) {
				CString ts;
				ts.Format(L"addr:%04X,n:%d,SCROLL_REG:%d,line:%d,line_t:%d,pos:%d,color_n:%d,tn:%X,nt_index:%d,N_TABLE_INDEX:%d", 
					0x2400 + n, n,SCROLL_REG[1], line, line_t, pos, color_n, tn, nt_index, N_TABLE_INDEX);
				//MessageBox(NULL, ts, L"t", MB_OK);
			}
			if (0 && (tn == 0x5E)) {
				CString ts;
				ts.Format(L"L:%0X,H:%0X,ADDR:%04X,POS:%d,I:%d,t:%d,group:%d", title_low, title_high, 0x1000 + (tn * 16), pos, i,
					(title_high >> i) & 0x01, group);
				//MessageBox(NULL, ts, L"t", MB_OK);
			}
			//color_n = 0x3;
			/*CString ts;
			ts.Format(L"tl:%d, th:%d, tn:%d, color_n:%x", title_low, title_high, tn, color_n);
			MessageBox(NULL, ts, L"t", MB_OK);*/
			int color = this->rgb(color_n);
			//color = RGB(0x80, 0x80, 0x80);
			//填充画面
			images[index++] = (color >> 16) & 0xff;
			images[index++] = (color >> 8) & 0xff;
			images[index++] = (color >> 0) & 0xff;
			images[index++] = 0;
			/*if (dc) {
				CPen pen(PS_SOLID, 1, this->rgb(color) + line);//创建一个虚线线条，宽度为1，红色的画笔对象  
				CPen* pOldPen = dc->SelectObject(&pen);//将画笔对象选入到设备描述表中 
				POINT ps = {sx, line};
				POINT pe = {sx + 1, line};
				dc->MoveTo(ps);
				dc->LineTo(pe);
				dc->SelectObject(pOldPen);
				pen.DeleteObject();
				if (line == 60) {
					MessageBox(NULL, L"60", L"t", MB_OK);
				}
			}*/
			sx += 1;
		}
		n++;
	}
	for (int j = 252; j > 0; j -= 4) {
		if (SRAMIN(SRAM[j], SPR_SIZE, line)) {
			index = line * 256 * 4 + (SRAM[j + 3] * 4);
			//字幕起始地址
			byte tn = SRAM[j + 1];
			byte* addr = SPRA + (tn * 16);
			byte k = line - SRAM[j];
			if (line < (SRAM[j] + 8)) {
				k = line - SRAM[j];
			}
			else {
				//addr = BGA +  (tn * 16);
				k =  7 - (line - 8 - SRAM[j]);
			}
			byte group = SRAM[j + 2] & 0x03;
			//低位字节
			byte title_low = *(addr + k);
			//高位字节
			byte title_high = *(addr + k + 8);
			//每两位表示一个像素在颜色组中的位置 [前8位是低位，后8位是高位]
			int init_v = 7, step_v = -1;
			for (int i = init_v; i >= 0 && i < 8; i += step_v) {
				//在调色板组中的位置
				byte pos = (title_low >> i) & 0x01; //低位
				pos |= ((title_high >> i) & 0x01) << 1; //高位
				pos &= 0x3; //只有两位 
							//获取颜色 *(BGC_TABLE + pos) 
				byte color_n = *BGC_TABLE;
				if (pos) {
					//if (pos == 0x1 || pos == 0x2) pos = 0x3;
					pos |= group << 2;
					color_n = *(SPR_TABLE + pos);
				}
				//color_n = 0x3;
				/*CString ts;
				ts.Format(L"tl:%d, th:%d, tn:%d, color_n:%x", title_low, title_high, tn, color_n);
				MessageBox(NULL, ts, L"t", MB_OK);*/
				int color = this->rgb(color_n);
				//color = RGB(0x80, 0x80, 0x80);
				//填充画面
				images[index++] = (color >> 16) & 0xff;
				images[index++] = (color >> 8)  & 0xff;
				images[index++] = (color >> 0)  & 0xff;
				images[index++] = 0;
			}
		}
	}
	if (line == 239) {
		CString ts;
		ts.Format(L"vblank, line:%d", line);
		//MessageBox(NULL, ts, L"t", MB_OK);
		SET_VBLANK();
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