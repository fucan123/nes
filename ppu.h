#pragma once

#ifndef PPUH
#define PPUH

class PPU {
public:
	//����PPU�ڴ�
	byte  MEM[0x7fff + 1];

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

	CPaintDC* dc;
public:
	PPU();
	//װ�ؿ���ͼ������
	void load(char*, size_t);
	void showBG();
	//����һ��ɨ����(�ֱ���256*240��һ��240��) line=Ҫ���Ƶ��߱��
	void inline scanfLine(byte line);
	int inline rgb(byte);
};

#endif // !PPUH
