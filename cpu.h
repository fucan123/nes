#pragma once

#include <stdio.h>
#include <iostream>
#include "ppu.h"

#ifndef CPUH
#define CPUH

#define DEBUGDLG(p) ((CDebugDlg*)p)

#define CPUSUC(code) (code == 0) 
#define CPUERR(code) (code > 0)
//����Ƿ�Ϊ0
#define ISZ(v) (v == 0)
//����Ƿ���Ҫ��λ
#define ISC(v) (v > 0xff)
//����Ƿ��Ǹ���
#define ISN(v) (v > 0x7f)
//����Ƿ����[���λΪ1��ô��Ϊ���]
#define ISV(v) (v > 0x7f)

typedef struct cpu6502 {
	byte A;  //8λ�ۼӼĴ���
	byte X;  //8λ��ַ�Ĵ���X
	byte Y;  //8λ��ַ�Ĵ���Y
	/*
	8λ��־�Ĵ���
	0-C��λλ:�н�λΪ1��1-Z���־λ��ִ�н��Ϊ0��λΪ1��2-I�жϱ��λ��Ϊ1��ֹ�жϷ������жϲ���Ӱ��
	3-Dλ��4-Bλ��5-������
	6-V������λ����ִ�н�������λΪ1��7-N�������λ�������Ϊ������λΪ1��
	*/
	byte P;  //8λ��־�Ĵ���
	byte S;  //8λ��ջָ�룬Ĭ��Ϊ0xff[�C��Ϊ0x100-0x1ff, ֻ�洢�˵��ֽ�, ��Ҫ��0x100], ��ջ-1, ��ջ+1
	word PC; //16λ����ָ���ַ�Ĵ������洢����һ��ָ��ִ�е�λ��
} CPU6502;

enum CPU6502_CODE { //M��ʾ��̬ȡ�õ�ֵ
	CERR=-1,  //����
	NOP = 0,  //�ղ���
	ADC = 1,  //����λ�ӷ����� A+M+C->A
	INC = 2,  //+1���� M+1->M
	INX = 3,  //��ַ�Ĵ���X+1 X+1->X
	INY = 4,  //��ַ�Ĵ���Y+1 Y+1->Y
	SBC = 5,  //����λ���� A-M-C->A
	DEC = 6,  //-1���� M-1->M
	DEX = 7,  //��ַ�Ĵ���X-1 X-1->X
	DEY = 8,  //��ַ�Ĵ���Y-1 Y-1->Y
	AND = 9,  //�߼��� A&M->A
	EOR = 10, //�߼���� A^M->A
	ORA = 11, //�߼��� A|M->A
	BIT = 12, //λ���� A&M �����6λ->V 7λ->N
	ASL = 13, //����һλ ���λ��C��־λ
	LSR = 14, //����һλ ���λ��C���λ
	ROL = 15, //����һλ ��ͬC��־λ �����λ��C��־λ C��־λ�����λ
	ROR = 16, //����һλ ��ͬC��־λ �����λ��C���λ C���λ�����λ
	CLC = 17, //��λ��־C��0
	CLD = 18, //ʮ����λD��0
	CLI = 19, //�ж�λI��0
	CLV = 20, //���λ��0
	SEC = 21, //���ý�λC
	SED = 22, //����ʮ����λD
	SEI = 23, //�����ж�λI
	CMP = 24, //�Ƚ� A-M ����0��ô��Zλ1��������ô��Cλ1
	CPX = 25, //�Ƚ� X-M ����0��ô��Zλ1��������ô��Cλ1
	CPY = 26, //�Ƚ� Y-M ����0��ô��Zλ1��������ô��Cλ1
	LDA = 27, //���ۼ���A M->A
	LDX = 28, //�ͼĴ���X M->X
	LDY = 29, //�ͼĴ���Y M->Y
	STA = 31, //���ۼ���A A->M
	STX = 32, //��Ĵ���X X->M
	STY = 33, //��Ĵ���Y Y->M
	TAX = 34, //A->X
	TAY = 35, //A->Y
	TSX = 36, //S->X
	TXS = 37, //X->S
	TXA = 38, //X->A
	TYA = 39, //Y->A
	PHA = 40, //�ۼ���A��ջ
	PHP = 41, //��ǼĴ���P��ջ
	PLA = 42, //��ջ���ۼ���A
	PLP = 43, //��ջ����־�Ĵ���P
	BEQ = 44, //��־Zλ=1 ת��
	BNE = 45, //��־Zλ=0 ת��
	BCS = 46, //��־Cλ=1 ת��
	BCC = 47, //��־Cλ=0 ת��
	BMI = 48, //��־Nλ=1 ת��
	BPL = 49, //��־Nλ=0 ת��
	BVS = 50, //��־Vλ=1 ת��
	BVC = 51, //��־Vλ=0 ת��
};

/*
Ѱַ��ʽ��
1.��������ֱ�Ӹ�������LDA #0xA 2�ֽ�
2.ֱ��Ѱַ[����Ѱַ]��LDA 0x00A0 ȡ0x00A0��ַ�ϵ�ֵ 3�ֽ�
3.�Ĵ���Xֱ�ӱ�ַ[���Ա�ַ] LDA 0x00A0,X addr=0x00A0+X ֵ=addr��ַ�ϵ�ֵ 3�ֽ�
4.�Ĵ���Yֱ�ӱ�ַ[���Ա�ַ] LDA 0x00A0,Y addr=0x00A0+Y ֵ=addr��ַ�ϵ�ֵ 3�ֽ�
5.��ҳѰַ��LDA 0xA0 ȥ��ַ0xA0�ϵ�ֵ ��ҳ��Χ(0x00-0xff) 2�ֽ� [���������Ѱַ��ʽ]
6.�Ĵ���X��ҳ��ַ  LDA 0xA0,X addr=0xA0+X ֵ=addr��ַ�ϵ�ֵ 2�ֽ�
7.�Ĵ���Y��ҳ��ַ  LDA 0xA0,Y addr=0xA0+Y ֵ=addr��ַ�ϵ�ֵ 2�ֽ�
8.���Ѱַֻ����JMP��תָ�� JMP 0x00A0 A1=0x00A0��8λ0xCD A2=0x00A0+1��ֵַ�õ���8λ)0xAB ֵ=0xABCD��ַ�ϵ�ֵ 3�ֽ�
9.�ȱ�ַX����Ѱַ[ֻ������ҳ] LDA (0xA0,X) A1=0xA0+X��ֵַ�õ���8λ0xCD A2=0xA0+X+1��ֵַ�õ���8λ)0xAB ֵ=0xABCD��ַ�ϵ�ֵ 2�ֽ�
10.��Ӻ��ַYѰַ[ֻ������ҳ] LDA (0xA0),Y A1=0xA0��ֵַ�õ���8λ0xCD A2=0xA0+1��ֵַ�õ���8λ)0xAB ֵ=0xABCD+Y��ַ�ϵ�ֵ 2�ֽ�
*/
enum CPU6502_MODE {
	M_0      = 0,  //0
	M_Q      = 1,  //������
	M_ABS    = 2,  //����Ѱַ
	M_X_ABS  = 3,  //�Ĵ���Xֱ�ӱ�ַ
	M_Y_ABS  = 4,  //�Ĵ���Yֱ�ӱ�ַ
	M_ZERO   = 5,  //��ҳѰַ
	M_X_ZERO = 6,  //�Ĵ���X��ҳ��ַ
	M_Y_ZERO = 7,  //�Ĵ���Y��ҳ��ַ
	M_IDA    = 8,  //���Ѱַ
	M_X_IDA  = 9,  //�ȱ�ַX����Ѱַ
	M_IDA_Y  = 10, //��Ӻ��ַYѰַ
	M_A      = 11, //�ۼ�������
	M_OFT    = 12, //ƫ�Ʋ���
};

enum CPU_ECODE {
	CPU_SUC    = 0, //û�д�
	OP_ERR     = 1, //ָ�����
};

class CPU {
public:
	CPU6502    R;
	word       DT;
	word       WT;
	byte       NF_TABLE[256];
	//ָ��ִ������
	int        EXEC_CYCLE;
	//����ִ������
	int        TOTAL_CYCLE;
	//CPU�ڴ�ӳ��, ֻ��Ѱַ��0xffff
	byte       MEM[0xffff + 1];
	byte       PPU[0x7fff + 1];
	//�C��
	byte*      STACK;
	//ָ���
	byte       opsize;
	struct {
		CPU_ECODE code;
		byte      value;
		char*     err_str;
	}          err;
	//�������еĵ�ַ
	word  run_addr;
	char  hex_str[8];
	char  asm_str[64];
	char  remark[64];

	int exec_opnum;
	int opnum;

	word pcp;

	byte* images;

	bool test_reg6;
public:
	CListCtrl* clist;
	HWND       dbgdlg;
	int        lrow;

	bool       pause;
	bool       step;
	bool       show_asm;

	CPU();
	void load(char*, size_t, char*, size_t);
	void reset();
	void run(int);
	int exec(int request_cycles);

	byte read(word addr);
	CPU6502_CODE opcode(byte);
	byte value(CPU6502_MODE, word* paddr=NULL);
	void write(word addr, byte value);

	void NMI();
	void inline ADC(CPU6502_MODE);
	void inline INC(CPU6502_MODE);
	void inline INX();
	void inline INY();
	void inline SBC(CPU6502_MODE);
	void inline DEC(CPU6502_MODE);
	void inline DEX();
	void inline DEY();
	void inline AND(CPU6502_MODE);
	void inline EOR(CPU6502_MODE);
	void inline ORA(CPU6502_MODE);
	void inline BIT(CPU6502_MODE);
	void inline ASL(CPU6502_MODE);
	void inline LSR(CPU6502_MODE);
	void inline ROL(CPU6502_MODE);
	void inline ROR(CPU6502_MODE);
	void inline JMP(CPU6502_MODE);
	void inline JSR();
	void inline RTS();
	void inline BRK();
	void inline RTI();
	void inline CMP(CPU6502_MODE);
	void inline CPX(CPU6502_MODE);
	void inline CPY(CPU6502_MODE);
	void inline LDA(CPU6502_MODE);
	void inline LDX(CPU6502_MODE);
	void inline LDY(CPU6502_MODE);
	void inline STA(CPU6502_MODE);
	void inline STX(CPU6502_MODE);
	void inline STY(CPU6502_MODE);
	void inline TAX();
	void inline TAY();
	void inline TSX();
	void inline TXA();
	void inline TXS();
	void inline TYA();
	void inline PHA();
	void inline PHP();
	void inline PLA();
	void inline PLP();
	void inline NOP();

	void inline BCC();
	void inline BCS();
	void inline BNE();
	void inline BEQ();
	void inline BPL();
	void inline BMI();
	void inline BVC();
	void inline BVS();
	void inline BJMP(bool jmp=false);
	
	void inline SEC(); //����C��λ
	void inline SEZ(); //����Zλ���Ϊ0
	void inline SEI(); //�����ж�λ[��ֹ�ж�]
	void inline SED(); //����ʮ����λ
	void inline SEV(); //�������λ
	void inline SEN(); //���ø���λ
	
	void inline CLC(); //���C��λ
	void inline CLZ(); //���Zλ�����Ϊ0
	void inline CLI(); //����ж�λ[�����ж�]
	void inline CLD(); //���ʮ����λ
	void inline CLV(); //������λ
	void inline CLN(); //�������λ

	byte inline GEC(); //���C��λ
	byte inline GEZ(); //���Zλ�����Ϊ0
	byte inline GEI(); //����ж�λ[�����ж�]
	byte inline GED(); //���ʮ����λ
	byte inline GEV(); //������λ
	byte inline GEN(); //��ø���λ


	void setPause(bool);
	void setStep(bool);
	void inline setAsmOpStr(const char*);
	void inline printAsm();
	void dumpError();
	void end() {
		//һ��ɨ����ʱ��
		double line_time = 1 / 50 / 312; //ÿ��50֡ һ֡312��ɨ����
		int line = 0; //�ڼ���ɨ����
		LARGE_INTEGER freq, stime, ctime;
		QueryPerformanceFrequency(&freq); //��ȡʱ��Ƶ��
		QueryPerformanceCounter(&stime); //113.6825
		while (true) {
			QueryPerformanceCounter(&ctime); //��ǰʱ��
			double dim = (double)(ctime.QuadPart - stime.QuadPart) / (double)freq.QuadPart;
			if (dim >= line_time) {
				//ִ��cpuָ�� 113.6825����
				if (line < 240) {
					//����ɨ����
				}
				else {
					//VBlank�ڼ�
				}
				if (++line == 312) //ȫ��312ɨ�����
					line = 0;
			}
		}

	}
	~CPU();
};
#endif