#pragma once

#ifndef PPUH
#define PPUH

class PPU {
public:
	//�Ĵ���6�ŵ�ַ
	word REG6_ADDR;
	word REG7_ADDR;
	word REG7_INC;
public:
	//�Ĵ���
	byte REG[8];

	//�Ĵ�����־
	byte REG_FLAG[8];

	byte SCROLL_REG[2];

	//����PPU�ڴ�
	byte  MEM[0x7fff + 1];

	//����[��ɫ]�ڴ�
	byte SRAM[0xff + 1];

	//����ͼ�ε�ַ
	byte* BGA;

	//����ͼ�ε�ַ
	byte* SPRA;

	//������0
	byte* N_TABLE[4];

	//���Ա�0
	byte* A_TABLE[4];

	byte N_TABLE_INDEX;

	byte N_TABLE_V;

	//������ɫ���ַ
	byte* BGC_TABLE;
	//�����ɫ���ַ
	byte* SPR_TABLE;

	//��ģ��Ӧ�����Ա��[ÿ������960����Ļ]
	byte  CAP_TBALE[960];

	byte  SPR_SIZE;

	bool IS_SET_REG6;
	bool IS_REG7_FIRST;
	//�Ƿ����ӦNMI�ж�
	bool IS_NMI;
public:
	PPU();
	//װ�ؿ���ͼ������
	void load(char*, size_t, byte);
	//��ȡ�Ĵ���
	byte readREG(byte addr);
	//д��Ĵ���
	void writeREG(byte addr, byte value);
	//д���ڴ�
	void writeMEM(word addr, byte value);
	//д�����ݵ������ڴ�
	void dmaSRAM(byte* src);
	//���Ʊ���
	void getBG(byte images[]);
	//����һ��ɨ����(�ֱ���256*240��һ��240��) line=Ҫ���Ƶ��߱��
	void inline scanfLine(byte line, byte images[]);
	int inline rgb(byte);
};

#endif // !PPUH
