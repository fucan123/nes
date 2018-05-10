#include "stdafx.h"
#include "cpu.h"

#ifndef CPUCPP
#define CPUCPP

// 6502 ��־λ
#define	C_FLAG		0x01		// ��λ
#define	Z_FLAG		0x02		// ���־�����Ϊ0����λΪ1��
#define	I_FLAG		0x04		// �ж�λ��1Ϊ��ֹ�жϣ�
#define	D_FLAG		0x08		// ʮ����λ
#define	B_FLAG		0x10		// �˳�
#define	R_FLAG		0x20		// ���� ����1
#define	V_FLAG		0x40		// ���
#define	N_FLAG		0x80		// ������

// ����ZN��־λ
#define	SET_ZN_FLAG(V)	{ R.P &= ~(Z_FLAG|N_FLAG); if((V)==0){ R.P |= Z_FLAG; } if((V)>0x7f){ R.P |= N_FLAG; } }
// �������������ñ�־λ
#define	TST_FLAG(F,V)	{ R.P &= ~(V); if((F)) R.P |= (V); }
//��ջ
#define PUSH(V) { STACK[R.S--] = (V); }
//��ջ
#define POP()   STACK[++R.S]

CPU::CPU() {
	memset(&R, 0, sizeof(CPU6502)); //���мĴ�����ʼ����0
	R.S = 0xff; //��ջָ��ָ��0xff
	R.P = Z_FLAG | R_FLAG;
	memset(&err, 0, sizeof(err));
	clist = NULL;
	lrow = 0;
	pause = false;
	step = false;
	//printf("sizeof err:%d", sizeof(err));
}

void CPU::load(char* m, size_t size, char* p, size_t psize) {
	memcpy(MEM, m, size);
	//�C��Ϊ0x100 - 0x1ff
	STACK = MEM + 0x100;
	memcpy(PPU, p, psize);
}

void CPU::reset() {
	//REST��������λ������0xfffc��0xfffd�˵�ַָ��Ŀռ�Ѱ��ָ�0xfffcΪ��8λ��0xfffdΪ��8λ��
	word opaddr = MEM[0xfffc] | (MEM[0xfffd] << 8);
	R.PC = opaddr;
	//run();
	//::MessageBox(NULL, pc, L"title", MB_OK);
	
	//::MessageBox(NULL, L"x", L"rest", MB_OK);
	/*printf("REST��ַ��0x%x, ָ��:0x%x\n", opaddr, MEM[opaddr]);
	opcode(MEM[R.PC]);
	printf("PC:0x%x\n", R.PC);
	opcode(MEM[R.PC]);
	printf("PC:0x%x\n", R.PC);
	opcode(MEM[R.PC]);*/
}
//����
void CPU::run() {
	for (int i = 0; i < 3; i++) {
		if (pause || step) {
			break;
		}
		lrow = i;
		//printf("PC:0x%x\n", R.PC);
		opcode(MEM[R.PC]);
	}
}

byte CPU::mapv(word addr) {
	return MEM[addr & 0xffff];
}

CPU6502_CODE CPU::opcode(byte opcode) {
	this->run_addr = R.PC;
	sprintf(this->hex_str, "%X ", opcode);
	//printf("opcode:0x%x\n", opcode);
	opsize = 0;
	switch (opcode) {
	case 0x69: //ADC #0x2B �ۼ���A+0x2B+Cλ 2�ֽ�
		this->adc(M_Q);
		break;
	case 0x65: //ADC 0x2B ��ҳ 2�ֽ�
		this->adc(M_ZERO);
		opcode += 2;
		break;
	case 0x75: //ADC 0x2B,X ��ҳX 2�ֽ�
		this->adc(M_X_ZERO);
		break;
	case 0x6D: //ADC 0x002B ���� 3�ֽ�
		this->adc(M_ABS);
		break;
	case 0x7D: //ADC 0x002B,X ����X 3�ֽ�
		this->adc(M_X_ABS);
		break;
	case 0x79: //ADC 0x002B,Y ����Y 3�ֽ�
		this->adc(M_Y_ABS);
		break;
	case 0x61: //ADC (0x2B,X) ��ַX��� 2�ֽ�
		this->adc(M_X_IDA);
		break;
	case 0x71: //ADC (0x2B),Y ��ӱ�ַY 2�ֽ�
		this->adc(M_IDA_Y);
		break;
	case 0xE6: //INC 0x2B ��ҳ 2�ֽ�
		this->inc(M_ZERO);
		break;
	case 0xF6: //INC 0x2B,X ��ҳX 2�ֽ�
		this->inc(M_X_ZERO);
		break;
	case 0xEE: //INC 0x002B ���� 3�ֽ�
		this->inc(M_ABS);
		break;
	case 0xFE: //INC 0x002B,X ����X 3�ֽ�
		this->inc(M_X_ABS);
		break;
	case 0xE8: //INX 1�ֽ�
		this->inx();
		break;
	case 0xC8: //INY 1�ֽ�
		this->iny();
		break;
	case 0xE9: //SBC #0x2B �ۼ���A-0x2B-Cλ 2�ֽ�
		this->sbc(M_Q);
		break;
	case 0xE5: //SBC 0x2B ��ҳ 2�ֽ�
		this->sbc(M_ZERO);
		break;
	case 0xF5: //SBC 0x2B,X ��ҳX 2�ֽ�
		this->sbc(M_X_ZERO);
		break;
	case 0xED: //SBC 0x002B ���� 3�ֽ�
		this->sbc(M_ABS);
		break;
	case 0xFD: //SBC 0x002B,X ����X 3�ֽ�
		this->sbc(M_X_ABS);
		break;
	case 0xF9: //SBC 0x002B,Y ����Y 3�ֽ�
		this->sbc(M_Y_ABS);
		break;
	case 0xE1: //SBC (0x2B,X) ��ַX��� 2�ֽ�
		this->sbc(M_X_IDA);
		break;
	case 0xF1: //SBC (0x2B),Y ��ӱ�ַY 2�ֽ�
		this->sbc(M_IDA_Y);
		break;
	case 0xC6: //DEC 0x2B ��ҳ 2�ֽ�
		this->dec(M_ZERO);
		break;
	case 0xD6: //DEC 0x2B,X ��ҳX 2�ֽ�
		this->dec(M_X_ZERO);
		break;
	case 0xCE: //DEC 0x002B ���� 3�ֽ�
		this->inc(M_ABS);
		break;
	case 0xDE: //DEC 0x002B,X ����X 3�ֽ�
		this->inc(M_X_ABS);
		break;
	case 0xCA: //DEX 1�ֽ�
		this->dex();
		break;
	case 0x88: //DEY 1�ֽ�
		this->dey();
		break;
	case 0x29: //AND #0x2B �ۼ���A&0x2B 2�ֽ�
		this->and(M_Q);
		break;
	case 0x25: //AND 0x2B ��ҳ 2�ֽ�
		this->and(M_ZERO);
		break;
	case 0x35: //AND 0x2B,X ��ҳX 2�ֽ�
		this->and(M_X_ZERO);
		break;
	case 0x2D: //AND 0x002B ���� 3�ֽ�
		this->and(M_ABS);
		break;
	case 0x3D: //AND 0x002B,X ����X 3�ֽ�
		this->and(M_X_ABS);
		break;
	case 0x39: //AND 0x002B,Y ����Y 3�ֽ�
		this->and(M_Y_ABS);
		break;
	case 0x21: //AND (0x2B,X) ��ַX��� 2�ֽ�
		this->and(M_X_IDA);
		break;
	case 0x31: //SBC (0x2B),Y ��ӱ�ַY 2�ֽ�
		this->and(M_IDA_Y);
		break;
	case 0x49: //EOR #0x2B �ۼ���A&0x2B 2�ֽ�
		this->eor(M_Q);
		break;
	case 0x45: //EOR 0x2B ��ҳ 2�ֽ�
		this->eor(M_ZERO);
		break;
	case 0x55: //EOR 0x2B,X ��ҳX 2�ֽ�
		this->eor(M_X_ZERO);
		break;
	case 0x4D: //EOR 0x002B ���� 3�ֽ�
		this->eor(M_ABS);
		break;
	case 0x5D: //EOR 0x002B,X ����X 3�ֽ�
		this->eor(M_X_ABS);
		break;
	case 0x59: //EOR 0x002B,Y ����Y 3�ֽ�
		this->eor(M_Y_ABS);
		break;
	case 0x41: //EOR (0x2B,X) ��ַX��� 2�ֽ�
		this->eor(M_X_IDA);
		break;
	case 0x51: //EOR (0x2B),Y ��ӱ�ַY 2�ֽ�
		this->eor(M_IDA_Y);
		break;
	/****ORA******/
	case 0x09: //ORA #0x2B �ۼ���A&0x2B 2�ֽ�
		this->ora(M_Q);
		break;
	case 0x05: //ORA 0x2B ��ҳ 2�ֽ�
		this->ora(M_ZERO);
		break;
	case 0x15: //ORA 0x2B,X ��ҳX 2�ֽ�
		this->ora(M_X_ZERO);
		break;
	case 0x0D: //ORA 0x002B ���� 3�ֽ�
		this->ora(M_ABS);
		break;
	case 0x1D: //ORA 0x002B,X ����X 3�ֽ�
		this->ora(M_X_ABS);
		break;
	case 0x19: //ORA 0x002B,Y ����Y 3�ֽ�
		this->ora(M_Y_ABS);
		break;
	case 0x01: //ORA (0x2B,X) ��ַX��� 2�ֽ�
		this->ora(M_X_IDA);
		break;
	case 0x11: //ORA (0x2B),Y ��ӱ�ַY 2�ֽ�
		this->ora(M_IDA_Y);
		break;
	/****ASL******/
	case 0x0A: //ASL A<<1 1�ֽ�
		this->asl(M_A);
		break;
	case 0x06: //ASL 0x2B ��ҳ 2�ֽ�
		this->asl(M_ZERO);
		break;
	case 0x16: //ASL 0x2B,X ��ҳX 2�ֽ�
		this->asl(M_X_ZERO);
		break;
	case 0x0E: //ASL 0x002b ���� 3�ֽ�
		this->asl(M_ABS);
		break;
	case 0x1E: //ASL 0x002b,X ����X 3�ֽ�
		this->asl(M_X_ABS);
		break;
	/****LSR******/
	case 0x4A: //LSR A>>1 1�ֽ�
		this->lsr(M_A);
		break;
	case 0x46: //LSR 0x2B ��ҳ 2�ֽ�
		this->lsr(M_ZERO);
		break;
	case 0x56: //LSR 0x2B,X ��ҳX 2�ֽ�
		this->lsr(M_X_ZERO);
		break;
	case 0x4E: //LSR 0x002b ���� 3�ֽ�
		this->lsr(M_ABS);
		break;
	case 0x5E: //LSR 0x002b,X ����X 3�ֽ�
		this->lsr(M_X_ABS);
		break;
	/****ROL******/
	case 0x2A: //ROL A<<1 1�ֽ�
		this->rol(M_A);
		break;
	case 0x26: //ROL 0x2B ��ҳ 2�ֽ�
		this->rol(M_ZERO);
		break;
	case 0x36: //ROL 0x2B,X ��ҳX 2�ֽ�
		this->rol(M_X_ZERO);
		break;
	case 0x2E: //ROL 0x002b ���� 3�ֽ�
		this->rol(M_ABS);
		break;
	case 0x3E: //ROL 0x002b,X ����X 3�ֽ�
		this->rol(M_X_ABS);
		break;
	/****ROR******/
	case 0x6A: //ROR A<<1 1�ֽ�
		this->rol(M_A);
		break;
	case 0x66: //ROR 0x2B ��ҳ 2�ֽ�
		this->rol(M_ZERO);
		break;
	case 0x76: //ROR 0x2B,X ��ҳX 2�ֽ�
		this->rol(M_X_ZERO);
		break;
	case 0x6E: //ROR 0x002b ���� 3�ֽ�
		this->rol(M_ABS);
		break;
	/******������תָ��******/
	case 0x7E: //ROR 0x002b,X ����X 3�ֽ�
		this->rol(M_X_ABS);
		break;
	case 0x90: //BCC C=0 ��ת 2�ֽ�
		this->bcc();
		break;
	case 0xB0: //BCS C=1 ��ת 2�ֽ�
		this->bcs();
		break;
	case 0xD0: //BNE Z=0 ��ת 2�ֽ�
		this->bne();
		break;
	case 0xF0: //BEQ Z=1 ��ת 2�ֽ�
		this->beq();
		break;
	case 0x10: //BPL N=0 ��ת 2�ֽ�
		this->bpl();
		break;
	case 0x30: //BMI N=1 ��ת 2�ֽ�
		this->bmi();
		break;
	case 0x50: //BVC V=0 ��ת 2�ֽ�
		this->bvc();
		break;
	case 0x70: //BVS V=1 ��ת 2�ֽ�
		this->bvs();
		break;
	/***JMP****/
	case 0x4C: //JMP 0x002B ���� 3�ֽ�
		this->jmp(M_ABS);
		break;
	case 0x6C: //JMP (0x002B) ��� 3�ֽ�
		this->jmp(M_IDA);
		break;
	/***JSR****/
	case 0x20: //JSR 0x002B ת�ӳ��� 3�ֽ�
		this->jsr();
		break;
	case 0x40: //RTI �жϷ��� 1�ֽ�
		this->rti();
		break;
	case 0x60: //RTS �ӳ��򷵻� 1�ֽ�
		this->rts();
		break;
	/*******���־λ******/
	case 0x18: //CLC 1�ֽ�
		this->clc();
		break;
	case 0xD8: //CLD 1�ֽ�
		this->cld();
		break;
	case 0x58: //CLI 1�ֽ�
		this->cli();
		break;
	case 0xB8: //CLV 1�ֽ�
		this->clv();
		break;
	/*******���ñ�־λ******/
	case 0x38: //SEC 1�ֽ�
		this->sec();
		break;
	case 0xF8: //SED 1�ֽ�
		this->sed();
		break;
	case 0x78: //SEI 1�ֽ�
		this->sei();
		break;
	/****CMP******/
	case 0xC9: //CMP #0x2B �ۼ���A��0x2B�Ƚ� 2�ֽ�
		this->cmp(M_Q);
		break;
	case 0xC5: //CMP 0x2B ��ҳ 2�ֽ�
		this->cmp(M_ZERO);
		break;
	case 0xD5: //CMP 0x2B,X ��ҳX 2�ֽ�
		this->cmp(M_X_ZERO);
		break;
	case 0xCD: //CMP 0x002B ���� 3�ֽ�
		this->cmp(M_ABS);
		break;
	case 0xDD: //CMP 0x002B,X ����X 3�ֽ�
		this->cmp(M_X_ABS);
		break;
	case 0xD9: //CMP 0x002B,Y ����Y 3�ֽ�
		this->cmp(M_Y_ABS);
		break;
	case 0xC1: //CMP (0x2B,X) ��ַX��� 2�ֽ�
		this->cmp(M_X_IDA);
		break;
	case 0xD1: //CMP (0x2B),Y ��ӱ�ַY 2�ֽ�
		this->cmp(M_IDA_Y);
		break;
	/****CPX******/
	case 0xE0: //CPX #0x2B �Ĵ���X��0x2B�Ƚ� 2�ֽ�
		this->cpx(M_Q);
		break;
	case 0xE4: //CPX 0x2B ��ҳ 2�ֽ�
		this->cpx(M_ZERO);
		break;
	case 0xEC: //CPX 0x002B ���� 3�ֽ�
		this->cpx(M_ABS);
		break;
	/****CPY******/
	case 0xC0: //CPY #0x2B �Ĵ���Y��0x2B�Ƚ� 2�ֽ�
		this->cpy(M_Q);
		break;
	case 0xC4: //CPY 0x2B ��ҳ 2�ֽ�
		this->cpy(M_ZERO);
		break;
	case 0xCC: //CPY 0x002B ���� 3�ֽ�
		this->cpy(M_ABS);
		break;
	/****LDA******/
	case 0xA9: //LDA #0x2B ���ۼ���A=0x2B 2�ֽ�
		this->lda(M_Q);
		break;
	case 0xA5: //LDA 0x2B ��ҳ 2�ֽ�
		this->lda(M_ZERO);
		break;
	case 0xB5: //LDA 0x2B,X ��ҳX 2�ֽ�
		this->lda(M_X_ZERO);
		break;
	case 0xAD: //LDA 0x002B ���� 3�ֽ�
		this->lda(M_ABS);
		break;
	case 0xBD: //LDA 0x002B,X ����X 3�ֽ�
		this->lda(M_X_ABS);
		break;
	case 0xB9: //LDA 0x002B,Y ����Y 3�ֽ�
		this->lda(M_Y_ABS);
		break;
	case 0xA1: //LDA (0x2B,X) ��ַX��� 2�ֽ�
		this->lda(M_X_IDA);
		break;
	case 0xB1: //LDA (0x2B),Y ��ӱ�ַY 2�ֽ�
		this->lda(M_IDA_Y);
		break;
	/****LDX******/
	case 0xA2: //LDX #0x2B �ͼĴ���X=0x2B 2�ֽ�
		this->ldx(M_Q);
		break;
	case 0xA6: //LDX 0x2B ��ҳ 2�ֽ�
		this->ldx(M_ZERO);
		break;
	case 0xB6: //LDX 0x2B,Y ��ҳY 2�ֽ�
		this->ldx(M_Y_ZERO);
		break;
	case 0xAE: //LDX 0x002B ���� 3�ֽ�
		this->ldx(M_ABS);
		break;
	case 0xBE: //LDX 0x002B,Y ����Y 3�ֽ�
		this->ldx(M_Y_ABS);
		break;
	/****LDY******/
	case 0xA0: //LDY #0x2B �ͼĴ���Y=0x2B 2�ֽ�
		this->ldy(M_Q);
		break;
	case 0xA4: //LDY 0x2B ��ҳ 2�ֽ�
		this->ldy(M_ZERO);
		break;
	case 0xB4: //LDY 0x2B,X ��ҳX 2�ֽ�
		this->ldy(M_X_ZERO);
		break;
	case 0xAC: //LDY 0x002B ���� 3�ֽ�
		this->ldy(M_ABS);
		break;
	case 0xBC: //LDY 0x002B,X ����X 3�ֽ�
		this->ldy(M_X_ABS);
		break;
	/****STA******/
	case 0x85: //STA 0x2B �ۼ���A���ڴ� ��ҳ 2�ֽ�
		this->sta(M_ZERO);
		break;
	case 0x95: //STA 0x2B,X ��ҳX 2�ֽ�
		this->sta(M_X_ZERO);
		break;
	case 0x8D: //STA 0x002B ���� 3�ֽ�
		this->sta(M_ABS);
		break;
	case 0x9D: //STA 0x002B,X ����X 3�ֽ�
		this->sta(M_X_ABS);
		break;
	case 0x99: //STA 0x002B,Y ����Y 3�ֽ�
		this->sta(M_Y_ABS);
		break;
	case 0x81: //STA (0x2B,X) ��ַX��� 2�ֽ�
		this->sta(M_X_IDA);
		break;
	case 0x91: //STA (0x2B),Y ��ӱ�ַY 2�ֽ�
		this->sta(M_IDA_Y);
		break;
	/****STX******/
	case 0x86: //STX 0x2B �Ĵ���X���ڴ�  ��ҳ 2�ֽ�
		this->stx(M_ZERO);
		break;
	case 0x96: //STX 0x2B,Y ��ҳY 2�ֽ�
		this->stx(M_Y_ZERO);
		break;
	case 0x8E: //STX 0x002B ���� 3�ֽ�
		this->stx(M_ABS);
		break;
	/****STY******/
	case 0x84: //STY 0x2B �Ĵ���X���ڴ�  ��ҳ 2�ֽ�
		this->sty(M_ZERO);
		break;
	case 0x94: //STY 0x2B,X ��ҳX 2�ֽ�
		this->sty(M_X_ZERO);
		break;
	case 0x8C: //STY 0x002B ���� 3�ֽ�
		this->sty(M_ABS);
		break;
	/*******�Ĵ������Ĵ���********/
	case 0xAA: //TAX A->X 1�ֽ�
		this->tax();
		break;
	case 0xA8: //TAY A->Y 1�ֽ�
		this->tay();
		break;
	case 0xBA: //TSX S->X 1�ֽ�
		this->tsx();
		break;
	case 0x8A: //TXA X->A 1�ֽ�
		this->txa();
		break;
	case 0x9A: //TXS X->S 1�ֽ�
		this->txs();
		break;
	case 0x98: //TYA Y->A 1�ֽ�
		this->tya();
		break;
	/*******��ջ����********/
	case 0x48: //PHA A��ջ 1�ֽ�
		this->pha();
		break;
	case 0x08: //PHP P��ջ 1�ֽ�
		this->php();
		break;
	case 0x68: //PLA ��ջ��A 1�ֽ�
		this->pla();
		break;
	case 0x28: //PLP ��ջ��P 1�ֽ�
		this->plp();
		break;
	case 0xEA: //NOP
		this->nop();
		break;
	default:
		sprintf(asm_str, "ָ�����");
		break;
	}
	this->printAsm();
	R.PC += opsize;
	return INX;
}

byte CPU::value(CPU6502_MODE mode, word* paddr) {
	err.code = CPU_SUC;
	word v = 0xff, addr;
	byte opd  = MEM[R.PC + 1]; //������1 [�ڲ���ָ�����һ���ֽ�]
	byte opd2 = MEM[R.PC + 2]; //������2 [�ڲ���ָ����¶����ֽ�]
	asm_str[3] = ' ';
	char* str = asm_str + 4;
	char* hstr = hex_str + 3;
	switch (mode) {
	case M_Q:
		addr = R.PC + 1;
		sprintf(str, "#0x%02X", opd);
		sprintf(hstr, "%02X", opd);
		opsize = 2;
		break;
	case M_ABS: 
		//��һ���ֽ�Ϊ��8λ���ڶ���Ϊ��8λ
		addr = opd | (opd2 << 8);
		sprintf(str, "[0x%04X]", addr);
		sprintf(hstr, "%02X%02X", opd, opd2);
		opsize = 3; 
		break;
	case M_X_ABS:
		//��һ���ֽ�Ϊ��8λ���ڶ���Ϊ��8λ
		addr  = opd | (opd2 << 8);
		sprintf(str, "0x%04X, X", addr);
		sprintf(hstr, "%02X%02X", opd, opd2);
		addr += R.X;
		opsize = 3;
		break;
	case M_Y_ABS:
		//��һ���ֽ�Ϊ��8λ���ڶ���Ϊ��8λ
		addr  = opd | (opd2 << 8);;
		sprintf(str, "0x%04X, Y", addr);
		sprintf(hstr, "%02X", opd);
		addr += R.Y;
		opsize = 3;
		break;
	case M_ZERO:
		addr = opd;
		sprintf(str, "[0x%02X]", addr);
		sprintf(hstr, "%02X", opd);
		opsize = 2;
		break;
	case M_X_ZERO:
		addr = opd + R.X;
		sprintf(hstr, "%02X", opd);
		opsize = 2;
		break;
	case M_Y_ZERO:
		addr = opd + R.Y;
		sprintf(hstr, "%02X", opd);
		opsize = 2;
		break;
	case M_IDA:
	{
		word tmp, tmp2;
		tmp = opd | (opd2 << 8);
		sprintf(str, "(0x%04X)", tmp);
		sprintf(hstr, "%02X%02X", opd, opd2);
		//��һ���ֽ�Ϊ��8λ���ڶ���Ϊ��8λ
		addr = MEM[tmp] | (MEM[tmp + 1] << 8);
		opsize = 3;
	}
		break;
	case M_X_IDA: 
	{
		//LDA (0xA0, X)
		word tmp, tmp2;
		tmp  = opd + R.X; //0xA0 + X
		tmp2 = opd + R.X + 1; //0xA0 + X + 1;
		//��һ���ֽ�Ϊ��8λ���ڶ���Ϊ��8λ
		addr = MEM[tmp] | (MEM[tmp2] << 8);
		sprintf(str, "(0x%02X, X)", opd);
		sprintf(hstr, "%02X", opd);
		opsize = 2;
	}
		
		break;
	case M_IDA_Y: 
		//LDA (0xA0), Y
		//��һ���ֽ�Ϊ��8λ���ڶ���Ϊ��8λ
		addr = MEM[opd] | ((MEM[opd + 1]) << 8);
		addr += R.Y;
		sprintf(str, "(0x%02X), Y", opd);
		sprintf(hstr, "%02X", opd);
		opsize = 2;
		break;
	case M_OFT:
		addr = opd;
		sprintf(str, "#0x%02X", addr);
		sprintf(hstr, "%02X", opd);
		opsize = 2;
		break;
	default:
		err.code = OP_ERR;
		asm_str[0] = 0;
		sprintf(asm_str, "ָ�����");
		opsize = 0;
		break;
	}
	if (CPUSUC(err.code)) {
		v = MEM[addr];
		if (paddr != NULL)
			*paddr = addr;
	}
	dumpError();
	return v;
}
/* ADC (NV----ZC) */
void CPU::adc(CPU6502_MODE mode) {
	this->setAsmOpStr("ADC");
	DT = this->value(mode); //���Ҫ�ۼӵ�ֵ
	if (CPUSUC(err.code)) { //ָ����Ч
		//A + M + C -> A
		WT = R.A + DT + (R.P & C_FLAG);				
		TST_FLAG(WT > 0xFF, C_FLAG);				
		TST_FLAG(((~(R.A^DT))&(R.A^WT) & 0x80), V_FLAG);
		//����ӽ���Ż��ۼ���A
		R.A = (byte)WT;
		SET_ZN_FLAG(R.A);
	}
}
/* INC (N-----Z-) */
void CPU::inc(CPU6502_MODE mode) {
	this->setAsmOpStr("INC");
	word addr;
	DT = this->value(mode, &addr); //���Ҫ�ۼӵ�ֵ
	if (CPUSUC(err.code)) { //ָ����Ч
		//M + 1 -> M
		DT++;
		//���д���ַ��
		MEM[addr] = (byte)DT;
		SET_ZN_FLAG(DT);
	}
}
/* INX (N-----Z-) */
void CPU::inx() {
	this->setAsmOpStr("INX");
	//X + 1 -> X
	R.X++;
	SET_ZN_FLAG(R.X);
	opsize = 1;
}
/* INY (N-----Z-) */
void CPU::iny() {
	this->setAsmOpStr("INY");
	//Y + 1 -> Y
	R.Y++;
	SET_ZN_FLAG(R.Y);
	opsize = 1;
}
/* SBC (NV----ZC) */
void CPU::sbc(CPU6502_MODE mode) {
	this->setAsmOpStr("SBC");
	DT = this->value(mode);
	//��Ҫ��C��λ
	WT = R.A - DT - ((~R.P) & C_FLAG);
	TST_FLAG(((R.A^DT) & (R.A^WT) & 0x80), V_FLAG);
	TST_FLAG(WT < 0x100, C_FLAG);
	R.A = (byte)WT;
	SET_ZN_FLAG(R.A);
}
/* DEX (N-----Z-) */
void CPU::dec(CPU6502_MODE mode) {
	this->setAsmOpStr("DEC");
	word addr;
	DT = this->value(mode, &addr); //���Ҫ����ֵ
	if (CPUSUC(err.code)) { //ָ����Ч
		//M - 1 -> M
		DT--;
		//���д���ַ��
		MEM[addr] = (byte)DT;
		SET_ZN_FLAG(DT);
	}
}
/* DEX (N-----Z-) */
void CPU::dex() {
	this->setAsmOpStr("DEX");
	R.X--;
	SET_ZN_FLAG(R.X);
	opsize = 1;
}
/* DEX (N-----Z-) */
void CPU::dey() {
	this->setAsmOpStr("DEY");
	R.Y--;
	SET_ZN_FLAG(R.Y);
	opsize = 1;
}
/* AND (N-----Z-) */
void CPU::and(CPU6502_MODE mode) {
	this->setAsmOpStr("AND");
	DT = this->value(mode);
	R.A &= DT;
	SET_ZN_FLAG(R.A);
}
/* EOR (N-----Z-) */
void CPU::eor(CPU6502_MODE mode) {
	this->setAsmOpStr("EOR");
	DT = this->value(mode);
	R.A ^= DT;
	SET_ZN_FLAG(R.A);
}
/* ORA (N-----Z-) */
void CPU::ora(CPU6502_MODE mode) {
	this->setAsmOpStr("ORA");
	DT = this->value(mode);
	R.A |= DT;
	SET_ZN_FLAG(R.A);
}
/* ASL (N-----ZC) */
void CPU::asl(CPU6502_MODE mode) {
	this->setAsmOpStr("ASL");
	if (mode == M_A) { //�ۼ���A����
		TST_FLAG(R.A & 0x80, C_FLAG);
		R.A <<= 1;
		SET_ZN_FLAG(R.A);
	}
	else {
		word addr;
		DT = this->value(mode, &addr);
		TST_FLAG(DT & 0x80, C_FLAG);
		DT <<= 1;
		MEM[addr] = (byte)DT;
		SET_ZN_FLAG(DT);
	}
}
/* LSR_A (N-----ZC) */
void CPU::lsr(CPU6502_MODE mode) {
	this->setAsmOpStr("LSR");
	if (mode == M_A) { //�ۼ���A����
		TST_FLAG(R.A & 0x01, C_FLAG);
		R.A >>= 1;
		SET_ZN_FLAG(R.A);
	}
	else {
		word addr;
		DT = this->value(mode, &addr);
		TST_FLAG(DT & 0x01, C_FLAG);
		DT >>= 1;
		MEM[addr] = (byte)DT;
		SET_ZN_FLAG(DT);
	}
}
/* ROL (N-----ZC) */
void CPU::rol(CPU6502_MODE mode) {
	this->setAsmOpStr("ROL");
	if (mode == M_A) { //�ۼ���A����
		if (R.P & C_FLAG) { //CλҲ������λ
			TST_FLAG(R.A & 0x80, C_FLAG);
			R.A = (R.A << 1) | 0x01;
		}
		else {
			TST_FLAG(R.A & 0x80, C_FLAG);
			R.A <<= 1;
		}
		SET_ZN_FLAG(R.A);
	}
	else {
		word addr;
		DT = this->value(mode, &addr);
		if (R.P & C_FLAG) { //CλҲ������λ �����λ
			TST_FLAG(DT & 0x80, C_FLAG);
			DT = (DT << 1) | 0x01;
		}
		else {
			TST_FLAG(R.A & 0x80, C_FLAG);
			DT <<= 1;;
		}
		MEM[addr] = (byte)DT;
		SET_ZN_FLAG(DT);
	}
}
/* ROR (N-----ZC) */
void CPU::ror(CPU6502_MODE mode) {
	this->setAsmOpStr("ROR");
	if (mode == M_A) { //�ۼ���A����
		if (R.P & C_FLAG) { //CλҲ������λ
			TST_FLAG(R.A & 0x01, C_FLAG);
			R.A = (R.A >> 1) | 0x80;
		}
		else {
			TST_FLAG(R.A & 0x01, C_FLAG);
			R.A >>= 1;
		}
		SET_ZN_FLAG(R.A);
	}
	else {
		word addr;
		DT = this->value(mode, &addr);
		if (R.P & C_FLAG) { //CλҲ������λ �����λ
			TST_FLAG(DT & 0x01, C_FLAG);
			DT = (DT >> 1) | 0x80;
		}
		else {
			TST_FLAG(R.A & 0x01, C_FLAG);
			DT <<= 1;;
		}
		MEM[addr] = (byte)DT;
		SET_ZN_FLAG(DT);
	}
}
/* BIT (NV----Z-) */
void CPU::bit(CPU6502_MODE mode) {
	this->setAsmOpStr("BIT");
	DT = this->value(mode);
	TST_FLAG((DT&R.A) == 0, Z_FLAG);
	TST_FLAG(DT & 0x80, N_FLAG);
	TST_FLAG(DT & 0x40, V_FLAG);
}
void CPU::jmp(CPU6502_MODE mode) {
	this->setAsmOpStr("JMP");
	word v = this->value(mode);
}
void CPU::jsr() {
	this->setAsmOpStr("JSR");
	opsize = 1;
}
void CPU::rti() {
	this->setAsmOpStr("RTI");
	opsize = 1;
}
void CPU::rts() {
	this->setAsmOpStr("RTS");
	opsize = 1;
}
/* CMP (N-----ZC) */
void CPU::cmp(CPU6502_MODE mode) {
	this->setAsmOpStr("CMP");
	DT = this->value(mode);
	WT = (word)R.A - (word)DT;
	TST_FLAG((WT & 0x8000) == 0, C_FLAG);
	SET_ZN_FLAG((byte)WT);
}
/* CPX (N-----ZC) */
void CPU::cpx(CPU6502_MODE mode) {
	this->setAsmOpStr("CPX");
	DT = this->value(mode);
	WT = (word)R.X - (word)DT;
	TST_FLAG((WT & 0x8000) == 0, C_FLAG);
	SET_ZN_FLAG((byte)WT);
}
/* CPY (N-----ZC) */
void CPU::cpy(CPU6502_MODE mode) {
	this->setAsmOpStr("CPY");
	DT = this->value(mode);
	WT = (word)R.Y - (word)DT;
	TST_FLAG((WT & 0x8000) == 0, C_FLAG);
	SET_ZN_FLAG((byte)WT);
}
/* LDA (N-----Z-) */
void CPU::lda(CPU6502_MODE mode) {
	this->setAsmOpStr("LDA");
	DT = this->value(mode);
	if (CPUSUC(err.code)) { //ָ����Ч
		//M -> A
		//::MessageBoxW(NULL, L"A", L"Title", MB_OK);
		R.A = DT;
		SET_ZN_FLAG(R.A);
	}
}
/* LDX (N-----Z-) */
void CPU::ldx(CPU6502_MODE mode) {
	this->setAsmOpStr("LDX");
	DT = this->value(mode);
	if (CPUSUC(err.code)) { //ָ����Ч
		//M -> X
		R.X = DT;
		SET_ZN_FLAG(R.X);
	}
}
/* LDY (N-----Z-) */
void CPU::ldy(CPU6502_MODE mode) {
	this->setAsmOpStr("LDY");
	DT = this->value(mode);
	if (CPUSUC(err.code)) { //ָ����Ч
		//M -> Y
		R.Y = DT;
		SET_ZN_FLAG(R.Y);
	}
}
/* STA (--------) */
void CPU::sta(CPU6502_MODE mode) {
	this->setAsmOpStr("STA");
	word addr;
	DT = this->value(mode, &addr);
	MEM[addr] = R.A;
}
/* STX (--------) */
void CPU::stx(CPU6502_MODE mode) {
	this->setAsmOpStr("STX");
	word addr;
	DT = this->value(mode, &addr);
	MEM[addr] = R.X;
}
/* STY (--------) */
void CPU::sty(CPU6502_MODE mode) {
	this->setAsmOpStr("STY");
	word addr;
	DT = this->value(mode, &addr);
	MEM[addr] = R.Y;
}
/* TAX (N-----Z-) */
void CPU::tax() {
	this->setAsmOpStr("TAX");
	R.X = R.A;
	SET_ZN_FLAG(R.X);
	opsize = 1;
}
/* TAY (N-----Z-) */
void CPU::tay() {
	this->setAsmOpStr("TAY");
	R.Y = R.A;
	SET_ZN_FLAG(R.Y);
	opsize = 1;
}
/* TSX (N-----Z-) */
void CPU::tsx() {
	this->setAsmOpStr("TSX");
	R.X = R.S;
	SET_ZN_FLAG(R.X);
	opsize = 1;
}
/* TXA (N-----Z-) */
void CPU::txa() {
	this->setAsmOpStr("TXA");
	R.A = R.X;
	SET_ZN_FLAG(R.A);
	opsize = 1;
}
/* TXS (N-----Z-) */
void CPU::txs() {
	this->setAsmOpStr("TXS");
	R.S = R.X;
	SET_ZN_FLAG(R.S);
	opsize = 1;
}
/* TYA (N-----Z-) */
void CPU::tya() {
	this->setAsmOpStr("TYA");
	R.A = R.Y;
	SET_ZN_FLAG(R.A);
	opsize = 1;
}
//CλΪ0 ��ת
void CPU::bcc() {
	this->setAsmOpStr("BCC");
	if (!(R.P & C_FLAG)) {
		this->bjmp();
	}
	opsize = 2;
}
//CλΪ1 ��ת
void CPU::bcs() {
	this->setAsmOpStr("BCS");
	if ((R.P & C_FLAG)) {
		this->bjmp();
	}
	opsize = 2;
}
//ZλΪ0 ��ת
void CPU::bne() {
	this->setAsmOpStr("BNE");
	this->bjmp(!(R.P & Z_FLAG));
}
//ZλΪ1 ��ת
void CPU::beq() {
	this->setAsmOpStr("BEQ");
	this->bjmp((R.P & Z_FLAG));
}
//NλΪ0 ��ת
void CPU::bpl() {
	this->setAsmOpStr("BPL");
	printf("P:%X, MEM:%X\n", R.P, MEM[0x2002]);
	this->bjmp(!(R.P & N_FLAG));
}
//NλΪ1 ��ת
void CPU::bmi() {
	this->setAsmOpStr("BMI");
	this->bjmp((R.P & N_FLAG));
}
//VλΪ0 ��ת
void CPU::bvc() {
	this->setAsmOpStr("BVC");
	this->bjmp(!(R.P & V_FLAG));
}
//vλΪ1 ��ת
void CPU::bvs() {
	this->setAsmOpStr("BVS");
	this->bjmp((R.P & V_FLAG));
}
//������ת
void CPU::bjmp(bool jmp) {
	word addr;
	DT = this->value(M_OFT, &addr);
	word a = R.PC;
	if (jmp) {
		R.PC = R.PC + (sbyte)addr;
		printf("R.PC:%X, DT:%X\n", R.PC, addr);
	}
}
// PHA A��ջ
void CPU::pha() {
	this->setAsmOpStr("PHA");
	PUSH(R.A);
	opsize = 1;
}
// PHP P��ջ
void CPU::php() {
	this->setAsmOpStr("PHP");
	PUSH(R.P | B_FLAG);
	opsize = 1;
}
// PLA (N-----Z-) ��ջ��A
void CPU::pla() {
	this->setAsmOpStr("PLA");
	R.A = POP();
	SET_ZN_FLAG(R.A);
	opsize = 1;
}
// PLP ��ջ��P
void CPU::plp() {
	this->setAsmOpStr("PLP");
	R.P = POP() | R_FLAG;
	opsize = 1;
}
void CPU::nop() {
	this->setAsmOpStr("NOP");
	opsize = 1;
}
//����C��λ
void CPU::sec() { 
	R.P |= 0x01;
	this->setAsmOpStr("SEC");
	opsize = 1;
}
//����Zλ���Ϊ0
void CPU::sez() { 
	R.P |= 0x02;
}
//�����ж�λ[��ֹ�ж�]
void CPU::sei() { 
	R.P |= 0x04; 
	this->setAsmOpStr("SEI");
	opsize = 1;
}
//����ʮ����λ
void CPU::sed() { 
	R.P |= 0x08;
	this->setAsmOpStr("SED");
	opsize = 1;
}
//�������λ
void CPU::sev() { R.P |= 0x40; }
//���ø���λ
void CPU::sen() { R.P |= 0x80; }

//���C��λ
void CPU::clc() { 
	R.P &= 0xfe;
	this->setAsmOpStr("CLC");
	opsize = 1;
}
//���Zλ�����Ϊ0
void CPU::clz() { R.P &= 0xfd; }
//����ж�λ[�����ж�]
void CPU::cli() { 
	R.P &= 0xfb;
	this->setAsmOpStr("CLI");
	opsize = 1;
}
//���ʮ����λ
void CPU::cld() { 
	R.P &= 0xf7;
	this->setAsmOpStr("CLD");
	opsize = 1;
}
//������λ
void CPU::clv() { 
	R.P &= 0xbf; 
	this->setAsmOpStr("CLV");
	opsize = 1;
}
//�������λ
void CPU::cln() { R.P &= 0x7f; }

//���C��λ
byte CPU::gec() { return R.P & 0x01; }
//���Zλ���
byte CPU::gez() { return (R.P >> 1) & 0x01; }
//����ж�λ
byte CPU::gei() { return (R.P >> 2) & 0x01; }
//���ʮ����λ
byte CPU::ged() { return (R.P >> 3) & 0x01; }
//������λ
byte CPU::gev() { return (R.P >> 6) & 0x01; }
//��ø���λ
byte CPU::gen() { return (R.P >> 7) & 0x01; }

void CPU::setPause(bool r) {
	pause = r;
	if (!r) {

	}
}

void CPU::setStep(bool r) {
	step = r;
	lrow++;
	//printf("PC:0x%x\n", R.PC);
	opcode(MEM[R.PC]);
}

void CPU::setAsmOpStr(const char* str) {
	asm_str[0] = str[0];
	asm_str[1] = str[1];
	asm_str[2] = str[2];
	asm_str[3] = 0;
}

void CPU::printAsm() {
	if (clist) {
		CString ra, hs(hex_str), as(asm_str);
		
		ra.Format(L"%X:", run_addr);
		//as.Format(L"%s", "fuck");
		//::MessageBox(NULL, as, L"title", MB_OK);
		/*****�������б�*****/
		int row = clist->GetItemCount();
		clist->InsertItem(row, ra); //һ��Ҫ���������ú������ʾ
		clist->SetItemText(row, 1, hs);
		clist->SetItemText(row, 2, as);
		clist->EnsureVisible(row, FALSE); //�������ײ�
		/*******��ʾ�Ĵ���ֵ*****/
		CString rs;
		rs.Format(L"CPU�Ĵ���   A:%02X   X:%02X   Y:%02X   S:%02X   P:%02X     PC:%02X", 
			R.A, R.X, R.Y, R.S, R.P, R.PC);
		SetWindowTextW(GetDlgItem(dbgdlg, 1003), rs);
		//MessageBox(NULL, L"set ok", L"t", MB_OK);
	}
	//printf("%s\n", asm_str);
}

void CPU::dumpError() {
	switch (err.code)
	{
	case OP_ERR:
		printf("ָ�����%d��������˵����%s\n", err.value, err.err_str);
		break;
	default:
		break;
	}
}

CPU::~CPU() {

}
#endif