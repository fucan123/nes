#pragma once

#include <stdio.h>
#include <iostream>

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
	//����������
	int        CYCLE;
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
public:
	CListCtrl* clist;
	HWND       dbgdlg;
	int        lrow;

	bool       pause;
	bool       step;

	CPU();
	void load(char*, size_t, char*, size_t);
	void reset();
	void run();

	byte mapv(word addr);
	CPU6502_CODE opcode(byte);
	byte value(CPU6502_MODE, word* paddr=NULL);

	void adc(CPU6502_MODE);
	void inc(CPU6502_MODE);
	void inx();
	void iny();
	void sbc(CPU6502_MODE);
	void dec(CPU6502_MODE);
	void dex();
	void dey();
	void and(CPU6502_MODE);
	void eor(CPU6502_MODE);
	void ora(CPU6502_MODE);
	void bit(CPU6502_MODE);
	void asl(CPU6502_MODE);
	void lsr(CPU6502_MODE);
	void rol(CPU6502_MODE);
	void ror(CPU6502_MODE);
	void jmp(CPU6502_MODE);
	void jsr();
	void rti();
	void rts();
	void cmp(CPU6502_MODE);
	void cpx(CPU6502_MODE);
	void cpy(CPU6502_MODE);
	void lda(CPU6502_MODE);
	void ldx(CPU6502_MODE);
	void ldy(CPU6502_MODE);
	void sta(CPU6502_MODE);
	void stx(CPU6502_MODE);
	void sty(CPU6502_MODE);
	void tax();
	void tay();
	void tsx();
	void txa();
	void txs();
	void tya();
	void pha();
	void php();
	void pla();
	void plp();
	void nop();

	void inline bcc();
	void inline bcs();
	void inline bne();
	void inline beq();
	void inline bpl();
	void inline bmi();
	void inline bvc();
	void inline bvs();
	void inline bjmp(bool jmp=false);
	
	void inline sec(); //����C��λ
	void inline sez(); //����Zλ���Ϊ0
	void inline sei(); //�����ж�λ[��ֹ�ж�]
	void inline sed(); //����ʮ����λ
	void inline sev(); //�������λ
	void inline sen(); //���ø���λ
	
	void inline clc(); //���C��λ
	void inline clz(); //���Zλ�����Ϊ0
	void inline cli(); //����ж�λ[�����ж�]
	void inline cld(); //���ʮ����λ
	void inline clv(); //������λ
	void inline cln(); //�������λ

	byte inline gec(); //���C��λ
	byte inline gez(); //���Zλ�����Ϊ0
	byte inline gei(); //����ж�λ[�����ж�]
	byte inline ged(); //���ʮ����λ
	byte inline gev(); //������λ
	byte inline gen(); //��ø���λ


	void setPause(bool);
	void setStep(bool);
	void setAsmOpStr(const char*);
	void printAsm();
	void dumpError();
	~CPU();
};
#endif