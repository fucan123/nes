#pragma once

#ifndef PPUH
#define PPUH

class PPU {
protected:
	//�Ĵ���6�ŵ�ַ
	word REG6_ADDR;
public:
	//�Ĵ���
	byte REG[8];

	//�Ĵ�����־
	byte REG_FLAG[8];

	//����PPU�ڴ�
	byte  MEM[0x7fff + 1];

	//����[��ɫ]�ڴ�
	byte SRAM[0xff + 1];

	//����ͼ�ε�ַ
	byte* BGA;

	//����ͼ�ε�ַ
	byte* SPRA;

	//������0
	byte* N_TABLE;

	//���Ա�0
	byte* A_TABLE;

	//������ɫ���ַ
	byte* BGC_TABLE;

	//��ģ��Ӧ�����Ա��[ÿ������960����Ļ]
	byte  CAP_TBALE[960];

	//�Ƿ����ӦNMI�ж�
	bool IS_NMI;
public:
	PPU();
	//װ�ؿ���ͼ������
	void load(char*, size_t);
	//��ȡ�Ĵ���
	byte readREG(byte addr);
	//д��Ĵ���
	void writeREG(byte addr, byte value);
	//д�����ݵ������ڴ�
	void dmaSRAM(byte* src);
	//���Ʊ���
	void getBG(byte images[]);
	//����һ��ɨ����(�ֱ���256*240��һ��240��) line=Ҫ���Ƶ��߱��
	void inline scanfLine(byte line, byte images[]);
	int inline rgb(byte);
};

#endif // !PPUH
