#pragma once
#include "../stdafx.h"
#include "PPU.h"

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
		byte line = i >> 5; //����32 ÿ��32����Ļ
		byte n = (i - (line * 32)) >> 2; //ÿ�и�4����Ļ����һ
		n += (line >> 2) * 8; //��4��+8 ��Ϊÿ��8����Ļ
		CAP_TBALE[i] = n; //��Ӧ���Ա��ı��
		//��һ����ǰ������Ļ(2*2)ռ0-1λ ��һ���к�������Ļռ2-3λ ��������ǰ������Ļ(2*2)ռ4-5λ �������к�������Ļռ6-7λ
		//�����������α��0,1,2,3
		byte bit = ((i - (line * 32)) & 0x03) >> 1; //����һ��������ռ�õı��(0-1)
		if ((line & 0x03) > 1) { //���˵�������
			bit += 2; //��һ����Ϊ0,1 �������о�Ϊ2,3
		}
		//��Ϊn�����63 ��λ2λ�ò��� ���ڱ���˱����Ӧ������λ���(0-3)
		CAP_TBALE[i] |= (bit << 6);
		//0-3(0), 4-7(2) 33-36(0)
	}
	SPR_SIZE = 8;
	REG7_INC = 0;
	IS_SET_REG6 = false;
	IS_REG7_FIRST = true;
	IS_NMI = false;
}

void PPU::load(char* m, size_t size, byte ntv) {
	memset(MEM, 0, 0x8000);
	memcpy(MEM, m, size);
	memset(SRAM, 0, 0xff + 1);
	memset(REG, 0, 8);
	memset(REG_FLAG, 0, 8);
	memset(HAND, 0, 2);
	memset(HAND_COUNT, 0, 2);
	memset(HAND_KEY, 0, 2 * 8);
	REG_FLAG[5] = 1;
	//memset(N_TABLE[0], 0x33, 1024);
	//memset(BGC_TABLE, 33, 16);
	REG6_ADDR = 0;
	N_TABLE_V = ntv;
	if (N_TABLE_V) { //��ֱ���� 
		N_TABLE[0] = N_TABLE[2] = &MEM[0x2000];
		N_TABLE[1] = N_TABLE[3] = &MEM[0x2400];
		A_TABLE[0] = A_TABLE[2] = &MEM[0x23C0];
		A_TABLE[1] = A_TABLE[3] = &MEM[0x27C0];
	}
	else { //ˮƽ����
		N_TABLE[0] = N_TABLE[1] = &MEM[0x2000];
		N_TABLE[2] = N_TABLE[3] = &MEM[0x2400];
		A_TABLE[0] = A_TABLE[1] = &MEM[0x23C0];
		A_TABLE[2] = A_TABLE[3] = &MEM[0x27C0];
	}
}
//��ȡ�Ĵ���
byte PPU::readREG(byte addr) {
	addr &= 0x07;
	//MessageBox(NULL, L"READ PPU REG!", L"t", MB_OK);
	byte value = REG[addr];
	switch (addr)
	{
	case 2: //���˿�2002 vblankλ��λ
		REG_FLAG[5] = 0; //��λ
		REG_FLAG[6] = 0; //��λ
		REG6_ADDR = 0;
		CLR_VBLANK();
		break;
	case 4: //��ȡ����RAM
		value = SRAM[REG[3]]; //�Ĵ������3��SRAM��ַ
		REG[3]++; //ÿ�η��ʵ�ַ+1
		break;
	case 7: //��ȡVRAM�ڴ� ��ַ��6�żĴ�������
		if (!REG_FLAG[7]) {
			REG7_ADDR = REG6_ADDR;
			REG_FLAG[7] = 1;
		}
		value = MEM[REG7_ADDR]; //��ȡVRAM�ڴ�
		if (!IS_REG7_FIRST) { //��һ�ζ���������Ч
			REG7_ADDR += (REG[0] & 0x04) ? 32 : 1; //�ڶ�λ����+32��+1
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
//д��Ĵ���
void PPU::writeREG(byte addr, byte value) {
	addr &= 0x07;
	//MessageBox(NULL, ts, L"t", MB_OK);
	switch (addr)
	{
	case 4: //д����RAM
		SRAM[REG[3]] = value; //д��SRAM
		REG[3]++; //ÿ�η��ʵ�ַ+1
		break;
	case 5: {
		SCROLL_REG[REG_FLAG[5]] = value;
		if (REG_FLAG[5] && value > 239) {
			//SCROLL_REG[1] = 239;
		}
		REG_FLAG[5] ^= 1;
		break;
	}
	case 6: { //���÷���VRAM�ĵ�ַ ��һ�θ�8λ �ڶ��ε�8λ
		if (!REG_FLAG[6]) { //д���λ
			REG6_ADDR = 0;
			REG6_ADDR |= (((word)value) << 8) & 0xff00;
			REG_FLAG[6] = 1;
			//ts.Format(L"д���λ:%X, addr:%X, addr2:%X", value, REG6_ADDR, REG6_ADDR & 0xff00);
			if (value > 0x70) {
				//MessageBox(NULL, ts, L"t", MB_OK);
			}
		}
		else { //д���λ
			REG6_ADDR |= (value & 0xff);
			REG_FLAG[6] = 0;
			//ts.Format(L"д���λ:%X, addr:%X", value, REG6_ADDR);
		}
		//MessageBox(NULL, ts, L"t", MB_OK);
		//CString ts;
		//ts.Format(L"REG6_ADDR:%X", REG6_ADDR);
		//MessageBox(NULL, ts, L"t", MB_OK);
		IS_SET_REG6 = true;
		break; }
	case 7: {
		if (IS_SET_REG6) {
			REG7_ADDR = REG6_ADDR;
			REG_FLAG[7] = 2;
			REG7_INC = 0;
			IS_SET_REG6 = false;
		}
		this->writeMEM(REG7_ADDR, value); //д��VRAM�ڴ�
		REG7_ADDR += (REG[0] & 0x04) ? 32 : 1; //�ڶ�λ����+32��+1
		break; }
	default:
		REG[addr] = value;
		if (addr == 0) {
			N_TABLE_INDEX = value & 0x03;
			//N_TABLE_V = value & 0x04 ? 1 : 0;
			if (N_TABLE_INDEX > 0 || N_TABLE_V > 0) {
				//CString ts;
				//ts.Format(L"index:%d, v:%d", N_TABLE_INDEX, N_TABLE_V);
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
			//���λ�����Ƿ���ӦNMI�ж�
			IS_NMI = value & 0x80 ? true : false;
		}
		break;
	}
}
//д���ڴ�
void PPU::writeMEM(word addr, byte value) {
	if (addr >= 0x2800 && addr < 0x2C00) { //����/���Ա�2(��0��ʼ)��ַ
		word dim = addr - 0x2800;
		*(N_TABLE[2] + dim) = value;
		if (dim < 0x10) {
			//CString ts;
			//ts.Format(L"2800 addr:%X, value:%X, SCROLL_REG:%x,%x", addr, value, SCROLL_REG[0], SCROLL_REG[1]);
			//MessageBox(NULL, ts, L"t", MB_OK);
		}
		
	}
	else if (addr >= 0x2C00 && addr < 0x3000) { //����/���Ա�3(��0��ʼ)��ַ
		word dim = addr - 0x2C00;
		*(N_TABLE[3] + dim) = value;
	}
	else if (addr == 0x3F00 || addr == 0x3F04 || addr == 0x3F08 || addr == 0x3F0C) {
		MEM[addr] = value;
		MEM[addr & 0x000F | 0x3F10] = value; //����0x3F1?
	}
	else if (addr == 0x3F10 || addr == 0x3F14 || addr == 0x3F18 || addr == 0x3F1C) {
		MEM[addr] = value;
		MEM[addr & 0x000F | 0x3F00] = value; //����0x3F1?
	}
	else {
		MEM[addr] = value;
	}
}
//д�����ݵ������ڴ�
void PPU::dmaSRAM(byte* src) {
	memcpy(SRAM, src, 0xff + 1);
}

void PPU::getBG(byte images[]) {
	//���л���(�ֱ���256*240��240��)
	for (int i = 0; i < 240; i++) {
		this->scanfLine(i, images);
	}
	SET_VBLANK();
	/*
	CPen pen(PS_SOLID, 1, RGB(0, 0, 0));//����һ����������������Ϊ1����ɫ�Ļ��ʶ���  
	CPen* pOldPen = dc->SelectObject(&pen);//�����ʶ���ѡ�뵽�豸�������� 
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
	word line_t = line;
	int index = line * 256 * 4;
	if (SCROLL_REG[0]) { //ˮƽ����
		//���ڻ������ĸ�����(�����������м��ű��) һ��8�� ÿ��32��
		byte ln = SCROLL_REG[0] >> 3;
		word n = (line_t >> 3) * 32 + ln;
		//��ģ�п�ʼ��ַ ռ2���ֽ� ÿ����ģ16�ֽ�
		word m = (line_t & 0x07);
		//һ��32����Ļ sx=x��ʼ����
		word e = n + 32 - ln, sx = 0;
		int si = 7 - (SCROLL_REG[0] & 0x07);
		
		//MessageBox(NULL, ts, L"t", MB_OK);
		//N_TABLE_INDEX = 0;
		while (n < e) {
			//��ģ���
			byte tn = N_TABLE[nt_index][n];
			//��Ļ��ʼ��ַ
			byte* addr = BGA + (tn * 16);
			//����ģ���� ��6λ�����Ա��
			byte attr = A_TABLE[nt_index][CAP_TBALE[n] & 0x3f];
			//��ɫ��
			byte group = (attr >> ((CAP_TBALE[n] >> 6) << 1)) & 0x03;
			//group = CAP_TBALE[n] >> 6;
			//������ɫ��ɫ���ַ 4�� ÿ��4�ֽ� ��16�ֽ�
			byte* col_addr = BGC_TABLE + (group * 4);
			//Ĭ����ɫ
			byte def_color = *BGC_TABLE;
			//��λ�ֽ�
			byte title_low = *(addr + m);
			//��λ�ֽ�
			byte title_high = *(addr + m + 8);
			//ÿ��λ��ʾһ����������ɫ���е�λ�� [ǰ8λ�ǵ�λ����8λ�Ǹ�λ]
			for (int i = si; i >= 0; i--) {
				si = 7;
				//�ڵ�ɫ�����е�λ��
				byte pos = (title_low >> i) & 0x01; //��λ
				pos |= ((title_high >> i) & 0x01) << 1; //��λ
				pos &= 0x3; //ֻ����λ 
							//��ȡ��ɫ *(BGC_TABLE + pos) 
				byte color_n = *BGC_TABLE;
				if (pos) {
					//if (pos == 0x1 || pos == 0x2) pos = 0x3;
					pos |= group << 2;
					color_n = *(BGC_TABLE + pos);
				}
				//color_n = 0x3;
				/*CString ts;
				ts.Format(L"tl:%d, th:%d, tn:%d, color_n:%x", title_low, title_high, tn, color_n);
				MessageBox(NULL, ts, L"t", MB_OK);*/
				int color = this->rgb(color_n);
				//color = RGB(0x80, 0x80, 0x80);
				//��仭��
				images[index++] = (color >> 16) & 0xff;
				images[index++] = (color >> 8) & 0xff;
				images[index++] = (color >> 0) & 0xff;
				images[index++] = pos;
			}
			n++;
		}
		/*-------------------------------------*/
		if (N_TABLE_INDEX == 0) {
			nt_index = 1;
		}
		else if (N_TABLE_INDEX == 1) {
			nt_index = 0;
		}
		else if (N_TABLE_INDEX == 2) {
			nt_index = 3;
		}
		else if (N_TABLE_INDEX == 3) {
			nt_index = 2;
		}
		//���ڻ������ĸ�����(�����������м��ű��) һ��8�� ÿ��32��
		ln = SCROLL_REG[0] >> 3;
		n = (line_t >> 3) * 32;
		//��ģ�п�ʼ��ַ ռ2���ֽ� ÿ����ģ16�ֽ�
		m = (line_t & 0x07);
		//һ��32����Ļ sx=x��ʼ����
		e = n + 1 + (ln), sx = 0;
		int ei = 0;
		//MessageBox(NULL, ts, L"t", MB_OK);
		//N_TABLE_INDEX = 0;
		while (n < e) {
			//��ģ���
			byte tn = N_TABLE[nt_index][n];
			//��Ļ��ʼ��ַ
			byte* addr = BGA + (tn * 16);
			//����ģ���� ��6λ�����Ա��
			byte attr = A_TABLE[nt_index][CAP_TBALE[n] & 0x3f];
			//��ɫ��
			byte group = (attr >> ((CAP_TBALE[n] >> 6) << 1)) & 0x03;
			//group = CAP_TBALE[n] >> 6;
			//������ɫ��ɫ���ַ 4�� ÿ��4�ֽ� ��16�ֽ�
			byte* col_addr = BGC_TABLE + (group * 4);
			//Ĭ����ɫ
			byte def_color = *BGC_TABLE;
			//��λ�ֽ�
			byte title_low = *(addr + m);
			//��λ�ֽ�
			byte title_high = *(addr + m + 8);
			//ÿ��λ��ʾһ����������ɫ���е�λ�� [ǰ8λ�ǵ�λ����8λ�Ǹ�λ]
			if (n == (e - 1)) {
				ei = 7 - (SCROLL_REG[0] & 0x07);
			}
			for (int i = 7; i >= ei; i--) {
				//�ڵ�ɫ�����е�λ��
				byte pos = (title_low >> i) & 0x01; //��λ
				pos |= ((title_high >> i) & 0x01) << 1; //��λ
				pos &= 0x3; //ֻ����λ 
							//��ȡ��ɫ *(BGC_TABLE + pos) 
				byte color_n = *BGC_TABLE;
				if (pos) {
					//if (pos == 0x1 || pos == 0x2) pos = 0x3;
					pos |= group << 2;
					color_n = *(BGC_TABLE + pos);
				}
				//color_n = 0x3;
				/*CString ts;
				ts.Format(L"tl:%d, th:%d, tn:%d, color_n:%x", title_low, title_high, tn, color_n);
				MessageBox(NULL, ts, L"t", MB_OK);*/
				int color = this->rgb(color_n);
				//color = RGB(0x80, 0x80, 0x80);
				//��仭��
				images[index++] = (color >> 16) & 0xff;
				images[index++] = (color >> 8) & 0xff;
				images[index++] = (color >> 0) & 0xff;
				images[index++] = pos;
			}
			n++;
		}
	}
	else { //��ֱ����
		byte vs = 0;
		if (SCROLL_REG[1] > 239) {
			vs = SCROLL_REG[1] - 240;
			//line_t += vs;
		}
		else {
			line_t += SCROLL_REG[1];
		}
		
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
			//MessageBox(NULL, t, L"t", MB_OK);
		}
		if (1 || line >= vs) {
			//���ڻ������ĸ�����(�����������м��ű��) һ��8�� ÿ��32��
			word n = (line_t >> 3) * 32;
			//��ģ�п�ʼ��ַ ռ2���ֽ� ÿ����ģ16�ֽ�
			word m = (line_t & 0x07);
			//һ��32����Ļ sx=x��ʼ����
			word e = n + 32, sx = 0;
			int index = line * 256 * 4;
			//MessageBox(NULL, ts, L"t", MB_OK);
			//N_TABLE_INDEX = 0;
			while (n < e) {
				//��ģ���
				byte tn = N_TABLE[nt_index][n];
				//��Ļ��ʼ��ַ
				byte* addr = BGA + (tn * 16);
				//����ģ���� ��6λ�����Ա��
				byte attr = A_TABLE[nt_index][CAP_TBALE[n] & 0x3f];
				//��ɫ��
				byte group = (attr >> ((CAP_TBALE[n] >> 6) << 1)) & 0x03;
				//group = CAP_TBALE[n] >> 6;
				//������ɫ��ɫ���ַ 4�� ÿ��4�ֽ� ��16�ֽ�
				byte* col_addr = BGC_TABLE + (group * 4);
				//Ĭ����ɫ
				byte def_color = *BGC_TABLE;
				//��λ�ֽ�
				byte title_low = *(addr + m);
				//��λ�ֽ�
				byte title_high = *(addr + m + 8);
				//ÿ��λ��ʾһ����������ɫ���е�λ�� [ǰ8λ�ǵ�λ����8λ�Ǹ�λ]
				for (int i = 7; i >= 0; i--) {
					//�ڵ�ɫ�����е�λ��
					byte pos = (title_low >> i) & 0x01; //��λ
					pos |= ((title_high >> i) & 0x01) << 1; //��λ
					pos &= 0x3; //ֻ����λ 
								//��ȡ��ɫ *(BGC_TABLE + pos) 
					if (tn == 0x2e || tn == 0xdd || tn == 0xde) {
						//CString ts;
						//ts.Format(L"tn:%X, line:%d, pos:%d", tn, line, pos);
						//MessageBox(NULL, ts, L"t", MB_OK);
					}
					byte color_n = *BGC_TABLE;
					if (pos) {
						//if (pos == 0x1 || pos == 0x2) pos = 0x3;
						pos |= group << 2;
						color_n = *(BGC_TABLE + pos);
					
					}
					//color_n = 0x3;
					/*CString ts;
					ts.Format(L"tl:%d, th:%d, tn:%d, color_n:%x", title_low, title_high, tn, color_n);
					MessageBox(NULL, ts, L"t", MB_OK);*/
					int color = this->rgb(color_n);
					//color = RGB(0x80, 0x80, 0x80);
					//��仭��
					images[index++] = (color >> 16) & 0xff;
					images[index++] = (color >> 8) & 0xff;
					images[index++] = (color >> 0) & 0xff;
					images[index++] = pos;
				}
				n++;
			}
		}
	}
	
	
	int spr_count = 0;
	for (int j = 252; j >= 0 && 1; j -= 4) {
		if (SRAM[j] && SRAMIN(SRAM[j] - 1, SPR_SIZE, line)) {
			spr_count++;
			index = line * 256 * 4 + (SRAM[j + 3] * 4);
			//��Ļ��ʼ��ַ
			byte tn = SRAM[j + 1];
			byte sta = SRAM[j + 2];
			byte* ptn = SPRA;
			if (SPR_SIZE == 16) {
				ptn = &MEM[(tn & 0x01) * 0x1000];
				if (line < (SRAM[j] - 1 + 8)) {
					if (tn & 0x01) 
						tn--;
				}
				else {
					if (!(tn & 0x01))
						tn++;
				}
			}
			byte* addr = ptn + (tn * 16);
			byte k = (line - (SRAM[j] - 1)) & 0x07;
			byte group = sta & 0x03;
			//ÿ��λ��ʾһ����������ɫ���е�λ�� [ǰ8λ�ǵ�λ����8λ�Ǹ�λ]
			int init_v = 7, step_v = -1;
			if (sta & 0x80) {
				k = 7 - k;
			}
			if (sta & 0x40) { //ˮƽ��ת
				init_v = 0; step_v = 1;
			}
			//��λ�ֽ�
			byte title_low = *(addr + k);
			//��λ�ֽ�
			byte title_high = *(addr + k + 8);
			for (int i = init_v; i >= 0 && i < 8; i += step_v) {
				//�ڵ�ɫ�����е�λ��
				byte pos = (title_low >> i) & 0x01; //��λ
				pos |= ((title_high >> i) & 0x01) << 1; //��λ
				pos &= 0x3; //ֻ����λ 
							//��ȡ��ɫ *(BGC_TABLE + pos) 
				byte color_n = *BGC_TABLE;
				if (!pos) {
					index += 4;
					continue;
				}
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
				
				//��仭��
				if (!(sta & 0x20)) {
					images[index++] = (color >> 16) & 0xff;
					images[index++] = (color >> 8) & 0xff;
					images[index++] = (color >> 0) & 0xff;
					images[index++] = pos;
				}
				else {
					if (images[index + 3] == 0) {
						images[index++] = (color >> 16) & 0xff;
						images[index++] = (color >> 8) & 0xff;
						images[index++] = (color >> 0) & 0xff;
						images[index++] = pos;
					}
				}
			}
		}
	}
	if (spr_count > 8) {
		REG[2] |= 0x20;
	}
	else {
		REG[2] &= ~(0x20);
	}
	/********���0�ž����Ƿ���ײ��������*******/
	REG[2] &= ~(0x40);
	if (SRAM[0] > 0 && SRAM[0] < 240) {
		byte spr0_x = SRAM[3];
		byte spr0_y = SRAM[0] - 1;
		//CString z;
		//z.Format(L"X:%d,Y:%d", SRAM[3], SRAM[0] - 1);

		//::MessageBox(NULL, z, L"T", MB_OK);
		for (int pline = 0; pline < 8; pline++) {
			for (int pcol = 0; pcol < 8; pcol++) {
				int f = (spr0_y + pline) * 256 * 4 + (spr0_x + pcol) * 4;
				if (images[f + 3] > 0) {
					REG[2] |= 0x40; //������ײλ
					goto end;
					//::MessageBox(NULL, L"0�ž�����ײ.", L"T", MB_OK);
				}
			}
		}
	}
	/********���0�ž����Ƿ���ײ��������*******/
end:
	if (line == 239) {
		//CString ts;
		//ts.Format(L"vblank, line:%d", line);
		//MessageBox(NULL, ts, L"t", MB_OK);
		REG[2] &= ~(0x40);
		SET_VBLANK();
	}
}
//��ɫ�干֧��64����ɫ
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