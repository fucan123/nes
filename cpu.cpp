#pragma once

#include "stdafx.h"
#include "cpu.h"
#include "ppu.h"

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

#define NMI_VECTOR  0xFFFA       // NMIִ�еĵ�ַλ��
#define REST_VECTOR 0xFFFC       // RESTִ�еĵ�ַλ��
#define IRQ_VECTOR  0xFFFE       // �ж�ִ�еĵ�ַλ��

#define ZERO_CYCLE()  EXEC_CYCLE = 0
#define ADD_CYCLE(V)  EXEC_CYCLE += (V)
// ���ñ�־λ
#define SET_FLAG(V) { R.P |= (V); }
// �����־λ
#define CLR_FLAG(V) { R.P &= ~(V); }
// ����ZN��־λ
#define	SET_ZN_FLAG(V)	{ R.P &= ~(Z_FLAG|N_FLAG); if((V)==0){ R.P |= Z_FLAG; } if((V)>0x7f){ R.P |= N_FLAG; } }
// �������������ñ�־λ
#define	TST_FLAG(F,V)	{ R.P &= ~(V); if((F)) R.P |= (V); }
//��ջ
#define PUSH(V) { STACK[R.S--] = (V); }
//��ջ
#define POP()   STACK[++R.S]

extern PPU g_PPU;

CPU::CPU( ) {
	memset(&R, 0, sizeof(CPU6502)); //���мĴ�����ʼ����0
	R.S = 0xff; //��ջָ��ָ��0xff
	R.P = Z_FLAG | R_FLAG;
	memset(&err, 0, sizeof(err));
	clist = NULL;
	lrow = 0;
	exec_opnum = 0;
	opnum = 0;
	pause = false;
	step = false;
	//printf("sizeof err:%d", sizeof(err));
}

void CPU::load(char* m, size_t size, char* p, size_t psize) {
	memcpy(MEM, m, size);
	//�C��Ϊ0x100 - 0x1ff
	STACK = MEM + 0x100;
	//memcpy(PPU, p, psize);
}

void CPU::reset() {
	//REST��������λ������0xfffc��0xfffd�˵�ַָ��Ŀռ�Ѱ��ָ�0xfffcΪ��8λ��0xfffdΪ��8λ��
	word opaddr = MEM[REST_VECTOR] | (MEM[REST_VECTOR + 1] << 8);
	R.PC = opaddr;
	CString xs;
	xs.Format(L"rest %d p:%d", opnum, this->pause);
	//MessageBox(NULL, xs, L"t", MB_OK);
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
void CPU::run(int num) {
	for (int i = 0; i < num; i++) {
		if (pause || step) {
			break;
		}
		lrow = i;
		//printf("PC:0x%x\n", R.PC);
		opcode(MEM[R.PC]);
	}
}
//ִ�� request_cyclesҪִ�е�����
int CPU::exec(int request_cycles) {
	//MessageBox(NULL, L"EXEC START!", L"t", MB_OK);
	int exec_cycles;
	while (opnum < exec_opnum && request_cycles > 0) {
		CString xs;
		ZERO_CYCLE();
		if (g_PPU.REG6_ADDR == 0x3F00) {
			test_reg6 = true;
		}
		if (test_reg6) {
			CString ts;
			ts.Format(L"EXEC:%X", g_PPU.REG6_ADDR);
			//MessageBox(NULL, ts, L"t", MB_OK);
		}
		if (opnum >= 0 && opnum <= 39) {
			xs.Format(L"exec opnum:%d p:%d", opnum, this->pause);
			//MessageBox(NULL, xs, L"t", MB_OK);
		}
		//opnum++;
		this->opcode(MEM[R.PC]);
		EXEC_CYCLE = 2;
		request_cycles -= EXEC_CYCLE;
		TOTAL_CYCLE += EXEC_CYCLE;
		
		xs.Format(L"all num:%d, opnum:%d",exec_opnum, opnum);
		//::SetWindowTextW(::GetDlgItem(dbgdlg, 1010), xs);
		if (opnum == 30 && opnum <= 39) {
			xs.Format(L"%d p:%d asm%s", opnum, this->pause, asm_str);
			//MessageBox(NULL, xs, L"t", MB_OK);
		}
	}
	return opnum;
}

CPU6502_CODE CPU::opcode(byte opcode) {
	CString xs;
	xs.Format(L"opnum----%d", opnum);
	//MessageBox(NULL, xs, L"t", MB_OK);
	opnum++;
	this->run_addr = R.PC;
	sprintf(this->hex_str, "%02X ", opcode);
	CString ts;
	ts.Format(L"opnum:%d", opnum);
	//MessageBox(NULL, ts, L"t", MB_OK);
	//return CERR;
	//printf("opcode:0x%x\n", opcode);
	opsize = 0;
	switch (opcode) {
	case 0x00: //BRK �ж�
		this->BRK();
		ADD_CYCLE(7);
		break;
	case 0x69: //ADC #0x2B �ۼ���A+0x2B+Cλ 2�ֽ�
		this->ADC(M_Q);
		ADD_CYCLE(2);
		break;
	case 0x65: //ADC 0x2B ��ҳ 2�ֽ�
		this->ADC(M_ZERO);
		ADD_CYCLE(3);
		opcode += 2;
		break;
	case 0x75: //ADC 0x2B,X ��ҳX 2�ֽ�
		this->ADC(M_X_ZERO);
		ADD_CYCLE(4);
		break;
	case 0x6D: //ADC 0x002B ���� 3�ֽ�
		this->ADC(M_ABS);
		ADD_CYCLE(4);
		break;
	case 0x7D: //ADC 0x002B,X ����X 3�ֽ�
		this->ADC(M_X_ABS);
		ADD_CYCLE(4);
		break;
	case 0x79: //ADC 0x002B,Y ����Y 3�ֽ�
		this->ADC(M_Y_ABS);
		ADD_CYCLE(4);
		break;
	case 0x61: //ADC (0x2B,X) ��ַX��� 2�ֽ�
		this->ADC(M_X_IDA);
		ADD_CYCLE(6);
		break;
	case 0x71: //ADC (0x2B),Y ��ӱ�ַY 2�ֽ�
		this->ADC(M_IDA_Y);
		ADD_CYCLE(5);
		break;
	case 0xE6: //INC 0x2B ��ҳ 2�ֽ�
		this->INC(M_ZERO);
		ADD_CYCLE(5);
		break;
	case 0xF6: //INC 0x2B,X ��ҳX 2�ֽ�
		this->INC(M_X_ZERO);
		ADD_CYCLE(6);
		break;
	case 0xEE: //INC 0x002B ���� 3�ֽ�
		this->INC(M_ABS);
		ADD_CYCLE(6);
		break;
	case 0xFE: //INC 0x002B,X ����X 3�ֽ�
		this->INC(M_X_ABS);
		ADD_CYCLE(7);
		break;
	case 0xE8: //INX 1�ֽ�
		this->INX();
		ADD_CYCLE(2);
		break;
	case 0xC8: //INY 1�ֽ�
		this->INY();
		ADD_CYCLE(2);
		break;
	case 0xE9: //SBC #0x2B �ۼ���A-0x2B-Cλ 2�ֽ�
		this->SBC(M_Q); ADD_CYCLE(2);
		break;
	case 0xE5: //SBC 0x2B ��ҳ 2�ֽ�
		this->SBC(M_ZERO); 
		ADD_CYCLE(3);
		break;
	case 0xF5: //SBC 0x2B,X ��ҳX 2�ֽ�
		this->SBC(M_X_ZERO);
		ADD_CYCLE(4);
		break;
	case 0xED: //SBC 0x002B ���� 3�ֽ�
		this->SBC(M_ABS);
		ADD_CYCLE(4);
		break;
	case 0xFD: //SBC 0x002B,X ����X 3�ֽ�
		this->SBC(M_X_ABS);
		ADD_CYCLE(4);
		break;
	case 0xF9: //SBC 0x002B,Y ����Y 3�ֽ�
		this->SBC(M_Y_ABS);
		ADD_CYCLE(4);
		break;
	case 0xE1: //SBC (0x2B,X) ��ַX��� 2�ֽ�
		this->SBC(M_X_IDA);
		ADD_CYCLE(6);
		break;
	case 0xF1: //SBC (0x2B),Y ��ӱ�ַY 2�ֽ�
		this->SBC(M_IDA_Y);
		ADD_CYCLE(5);
		break;
	case 0xC6: //DEC 0x2B ��ҳ 2�ֽ�
		this->DEC(M_ZERO);
		ADD_CYCLE(5);
		break;
	case 0xD6: //DEC 0x2B,X ��ҳX 2�ֽ�
		this->DEC(M_X_ZERO);
		ADD_CYCLE(6);
		break;
	case 0xCE: //DEC 0x002B ���� 3�ֽ�
		this->INC(M_ABS);
		ADD_CYCLE(6);
		break;
	case 0xDE: //DEC 0x002B,X ����X 3�ֽ�
		this->INC(M_X_ABS);
		ADD_CYCLE(7);
		break;
	case 0xCA: //DEX 1�ֽ�
		this->DEX();
		ADD_CYCLE(2);
		break;
	case 0x88: //DEY 1�ֽ�
		this->DEY();
		ADD_CYCLE(2);
		break;
	case 0x29: //AND #0x2B �ۼ���A&0x2B 2�ֽ�
		this->AND(M_Q);
		ADD_CYCLE(2);
		break;
	case 0x25: //AND 0x2B ��ҳ 2�ֽ�
		this->AND(M_ZERO);
		ADD_CYCLE(3);
		break;
	case 0x35: //AND 0x2B,X ��ҳX 2�ֽ�
		this->AND(M_X_ZERO);
		ADD_CYCLE(4);
		break;
	case 0x2D: //AND 0x002B ���� 3�ֽ�
		this->AND(M_ABS);
		ADD_CYCLE(4);
		break;
	case 0x3D: //AND 0x002B,X ����X 3�ֽ�
		this->AND(M_X_ABS);
		ADD_CYCLE(4);
		break;
	case 0x39: //AND 0x002B,Y ����Y 3�ֽ�
		this->AND(M_Y_ABS);
		ADD_CYCLE(4);
		break;
	case 0x21: //AND (0x2B,X) ��ַX��� 2�ֽ�
		this->AND(M_X_IDA);
		ADD_CYCLE(6);
		break;
	case 0x31: //AND (0x2B),Y ��ӱ�ַY 2�ֽ�
		this->AND(M_IDA_Y);
		ADD_CYCLE(5);
		break;
	case 0x49: //EOR #0x2B �ۼ���A&0x2B 2�ֽ�
		this->EOR(M_Q);
		ADD_CYCLE(2);
		break;
	case 0x45: //EOR 0x2B ��ҳ 2�ֽ�
		this->EOR(M_ZERO);
		ADD_CYCLE(3);
		break;
	case 0x55: //EOR 0x2B,X ��ҳX 2�ֽ�
		this->EOR(M_X_ZERO);
		ADD_CYCLE(4);
		break;
	case 0x4D: //EOR 0x002B ���� 3�ֽ�
		this->EOR(M_ABS);
		ADD_CYCLE(4);
		break;
	case 0x5D: //EOR 0x002B,X ����X 3�ֽ�
		this->EOR(M_X_ABS);
		ADD_CYCLE(4);
		break;
	case 0x59: //EOR 0x002B,Y ����Y 3�ֽ�
		this->EOR(M_Y_ABS);
		ADD_CYCLE(4);
		break;
	case 0x41: //EOR (0x2B,X) ��ַX��� 2�ֽ�
		this->EOR(M_X_IDA);
		ADD_CYCLE(6);
		break;
	case 0x51: //EOR (0x2B),Y ��ӱ�ַY 2�ֽ�
		this->EOR(M_IDA_Y);
		ADD_CYCLE(5);
		break;
	/****ORA******/
	case 0x09: //ORA #0x2B �ۼ���A&0x2B 2�ֽ�
		this->ORA(M_Q);
		ADD_CYCLE(2);
		break;
	case 0x05: //ORA 0x2B ��ҳ 2�ֽ�
		this->ORA(M_ZERO);
		ADD_CYCLE(3);
		break;
	case 0x15: //ORA 0x2B,X ��ҳX 2�ֽ�
		this->ORA(M_X_ZERO);
		ADD_CYCLE(4);
		break;
	case 0x0D: //ORA 0x002B ���� 3�ֽ�
		this->ORA(M_ABS);
		ADD_CYCLE(4);
		break;
	case 0x1D: //ORA 0x002B,X ����X 3�ֽ�
		this->ORA(M_X_ABS);
		ADD_CYCLE(4);
		break;
	case 0x19: //ORA 0x002B,Y ����Y 3�ֽ�
		this->ORA(M_Y_ABS);
		ADD_CYCLE(4);
		break;
	case 0x01: //ORA (0x2B,X) ��ַX��� 2�ֽ�
		this->ORA(M_X_IDA);
		ADD_CYCLE(6);
		break;
	case 0x11: //ORA (0x2B),Y ��ӱ�ַY 2�ֽ�
		this->ORA(M_IDA_Y);
		ADD_CYCLE(5);
		break;
	/****ASL******/
	case 0x0A: //ASL A<<1 1�ֽ�
		this->ASL(M_A);
		ADD_CYCLE(2);
		break;
	case 0x06: //ASL 0x2B ��ҳ 2�ֽ�
		this->ASL(M_ZERO);
		ADD_CYCLE(5);
		break;
	case 0x16: //ASL 0x2B,X ��ҳX 2�ֽ�
		this->ASL(M_X_ZERO);
		ADD_CYCLE(6);
		break;
	case 0x0E: //ASL 0x002b ���� 3�ֽ�
		this->ASL(M_ABS);
		ADD_CYCLE(6);
		break;
	case 0x1E: //ASL 0x002b,X ����X 3�ֽ�
		this->ASL(M_X_ABS);
		ADD_CYCLE(7);
		break;
	/****LSR******/
	case 0x4A: //LSR A>>1 1�ֽ�
		this->LSR(M_A);
		ADD_CYCLE(2);
		break;
	case 0x46: //LSR 0x2B ��ҳ 2�ֽ�
		this->LSR(M_ZERO);
		ADD_CYCLE(5);
		break;
	case 0x56: //LSR 0x2B,X ��ҳX 2�ֽ�
		this->LSR(M_X_ZERO);
		ADD_CYCLE(6);
		break;
	case 0x4E: //LSR 0x002b ���� 3�ֽ�
		this->LSR(M_ABS);
		ADD_CYCLE(6);
		break;
	case 0x5E: //LSR 0x002b,X ����X 3�ֽ�
		this->LSR(M_X_ABS);
		ADD_CYCLE(7);
		break;
	/****ROL******/
	case 0x2A: //ROL A<<1 1�ֽ�
		this->ROL(M_A);
		ADD_CYCLE(2);
		break;
	case 0x26: //ROL 0x2B ��ҳ 2�ֽ�
		this->ROL(M_ZERO);
		ADD_CYCLE(5);
		break;
	case 0x36: //ROL 0x2B,X ��ҳX 2�ֽ�
		this->ROL(M_X_ZERO);
		ADD_CYCLE(6);
		break;
	case 0x2E: //ROL 0x002b ���� 3�ֽ�
		this->ROL(M_ABS);
		ADD_CYCLE(6);
		break;
	case 0x3E: //ROL 0x002b,X ����X 3�ֽ�
		this->ROL(M_X_ABS);
		ADD_CYCLE(7);
		break;
	/****ROR******/
	case 0x6A: //ROR A<<1 1�ֽ�
		this->ROR(M_A);
		ADD_CYCLE(2);
		break;
	case 0x66: //ROR 0x2B ��ҳ 2�ֽ�
		this->ROR(M_ZERO);
		ADD_CYCLE(5);
		break;
	case 0x76: //ROR 0x2B,X ��ҳX 2�ֽ�
		this->ROR(M_X_ZERO);
		ADD_CYCLE(6);
		break;
	case 0x6E: //ROR 0x002b ���� 3�ֽ�
		this->ROR(M_ABS);
		ADD_CYCLE(6);
		break;
	case 0x7E: //ROR 0x002b,X ����X 3�ֽ�
		this->ROR(M_X_ABS);
		ADD_CYCLE(7);
		break;
	/******������תָ��******/
	case 0x90: //BCC C=0 ��ת 2�ֽ�
		this->BCC();
		ADD_CYCLE(2);
		break;
	case 0xB0: //BCS C=1 ��ת 2�ֽ�
		this->BCS();
		ADD_CYCLE(2);
		break;
	case 0xD0: //BNE Z=0 ��ת 2�ֽ�
		this->BNE();
		ADD_CYCLE(2);
		break;
	case 0xF0: //BEQ Z=1 ��ת 2�ֽ�
		this->BEQ();
		ADD_CYCLE(2);
		break;
	case 0x10: //BPL N=0 ��ת 2�ֽ�
		this->BPL();
		ADD_CYCLE(2);
		break;
	case 0x30: //BMI N=1 ��ת 2�ֽ�
		this->BMI();
		ADD_CYCLE(2);
		break;
	case 0x50: //BVC V=0 ��ת 2�ֽ�
		this->BVC();
		ADD_CYCLE(2);
		break;
	case 0x70: //BVS V=1 ��ת 2�ֽ�
		this->BVS();
		ADD_CYCLE(2);
		break;
	/***JMP****/
	case 0x4C: //JMP 0x002B ���� 3�ֽ�
		this->JMP(M_ABS);
		ADD_CYCLE(3);
		break;
	case 0x6C: //JMP (0x002B) ��� 3�ֽ�
		this->JMP(M_IDA);
		ADD_CYCLE(5);
		break;
	/***JSR****/
	case 0x20: //JSR 0x002B ת�ӳ��� 3�ֽ�
		this->JSR();
		ADD_CYCLE(6);
		break;
	case 0x40: //RTI �жϷ��� 1�ֽ�
		this->RTI();
		ADD_CYCLE(6);
		break;
	case 0x60: //RTS �ӳ��򷵻� 1�ֽ�
		this->RTS();
		ADD_CYCLE(6);
		break;
	/*******���־λ******/
	case 0x18: //CLC 1�ֽ�
		this->CLC();
		ADD_CYCLE(2);
		break;
	case 0xD8: //CLD 1�ֽ�
		this->CLD();
		ADD_CYCLE(2);
		break;
	case 0x58: //CLI 1�ֽ�
		this->CLI();
		ADD_CYCLE(2);
		break;
	case 0xB8: //CLV 1�ֽ�
		this->CLV();
		ADD_CYCLE(2);
		break;
	/*******���ñ�־λ******/
	case 0x38: //SEC 1�ֽ�
		this->SEC();
		ADD_CYCLE(2);
		break;
	case 0xF8: //SED 1�ֽ�
		this->SED();
		ADD_CYCLE(2);
		break;
	case 0x78: //SEI 1�ֽ�
		this->SEI();
		ADD_CYCLE(2);
		break;
	/****CMP******/
	case 0xC9: //CMP #0x2B �ۼ���A��0x2B�Ƚ� 2�ֽ�
		this->CMP(M_Q);
		ADD_CYCLE(2);
		break;
	case 0xC5: //CMP 0x2B ��ҳ 2�ֽ�
		this->CMP(M_ZERO);
		ADD_CYCLE(3);
		break;
	case 0xD5: //CMP 0x2B,X ��ҳX 2�ֽ�
		this->CMP(M_X_ZERO);
		ADD_CYCLE(4);
		break;
	case 0xCD: //CMP 0x002B ���� 3�ֽ�
		this->CMP(M_ABS);
		ADD_CYCLE(4);
		break;
	case 0xDD: //CMP 0x002B,X ����X 3�ֽ�
		this->CMP(M_X_ABS);
		ADD_CYCLE(4);
		break;
	case 0xD9: //CMP 0x002B,Y ����Y 3�ֽ�
		this->CMP(M_Y_ABS);
		ADD_CYCLE(4);
		break;
	case 0xC1: //CMP (0x2B,X) ��ַX��� 2�ֽ�
		this->CMP(M_X_IDA);
		ADD_CYCLE(6);
		break;
	case 0xD1: //CMP (0x2B),Y ��ӱ�ַY 2�ֽ�
		this->CMP(M_IDA_Y);
		ADD_CYCLE(5);
		break;
	/****CPX******/
	case 0xE0: //CPX #0x2B �Ĵ���X��0x2B�Ƚ� 2�ֽ�
		this->CPX(M_Q);
		ADD_CYCLE(2);
		break;
	case 0xE4: //CPX 0x2B ��ҳ 2�ֽ�
		this->CPX(M_ZERO);
		ADD_CYCLE(3);
		break;
	case 0xEC: //CPX 0x002B ���� 3�ֽ�
		this->CPX(M_ABS);
		ADD_CYCLE(4);
		break;
	/****CPY******/
	case 0xC0: //CPY #0x2B �Ĵ���Y��0x2B�Ƚ� 2�ֽ�
		this->CPY(M_Q);
		ADD_CYCLE(2);
		break;
	case 0xC4: //CPY 0x2B ��ҳ 2�ֽ�
		this->CPY(M_ZERO);
		ADD_CYCLE(3);
		break;
	case 0xCC: //CPY 0x002B ���� 3�ֽ�
		this->CPY(M_ABS);
		ADD_CYCLE(4);
		break;
	/****LDA******/
	case 0xA9: //LDA #0x2B ���ۼ���A=0x2B 2�ֽ�
		this->LDA(M_Q);
		ADD_CYCLE(2);
		break;
	case 0xA5: //LDA 0x2B ��ҳ 2�ֽ�
		this->LDA(M_ZERO);
		ADD_CYCLE(3);
		break;
	case 0xB5: //LDA 0x2B,X ��ҳX 2�ֽ�
		this->LDA(M_X_ZERO);
		ADD_CYCLE(4);
		break;
	case 0xAD: //LDA 0x002B ���� 3�ֽ�
		this->LDA(M_ABS);
		ADD_CYCLE(4);
		break;
	case 0xBD: //LDA 0x002B,X ����X 3�ֽ�
		this->LDA(M_X_ABS);
		ADD_CYCLE(4);
		break;
	case 0xB9: //LDA 0x002B,Y ����Y 3�ֽ�
		this->LDA(M_Y_ABS);
		ADD_CYCLE(4);
		break;
	case 0xA1: //LDA (0x2B,X) ��ַX��� 2�ֽ�
		this->LDA(M_X_IDA);
		ADD_CYCLE(6);
		break;
	case 0xB1: //LDA (0x2B),Y ��ӱ�ַY 2�ֽ�
		this->LDA(M_IDA_Y);
		ADD_CYCLE(5);
		break;
	/****LDX******/
	case 0xA2: //LDX #0x2B �ͼĴ���X=0x2B 2�ֽ�
		this->LDX(M_Q);
		ADD_CYCLE(2);
		break;
	case 0xA6: //LDX 0x2B ��ҳ 2�ֽ�
		this->LDX(M_ZERO);
		ADD_CYCLE(3);
		break;
	case 0xB6: //LDX 0x2B,Y ��ҳY 2�ֽ�
		this->LDX(M_Y_ZERO);
		ADD_CYCLE(4);
		break;
	case 0xAE: //LDX 0x002B ���� 3�ֽ�
		this->LDX(M_ABS);
		ADD_CYCLE(4);
		break;
	case 0xBE: //LDX 0x002B,Y ����Y 3�ֽ�
		this->LDX(M_Y_ABS);
		ADD_CYCLE(4);
		break;
	/****LDY******/
	case 0xA0: //LDY #0x2B �ͼĴ���Y=0x2B 2�ֽ�
		this->LDY(M_Q);
		ADD_CYCLE(2);
		break;
	case 0xA4: //LDY 0x2B ��ҳ 2�ֽ�
		this->LDY(M_ZERO);
		ADD_CYCLE(3);
		break;
	case 0xB4: //LDY 0x2B,X ��ҳX 2�ֽ�
		this->LDY(M_X_ZERO);
		ADD_CYCLE(4);
		break;
	case 0xAC: //LDY 0x002B ���� 3�ֽ�
		this->LDY(M_ABS);
		ADD_CYCLE(4);
		break;
	case 0xBC: //LDY 0x002B,X ����X 3�ֽ�
		this->LDY(M_X_ABS);
		ADD_CYCLE(4);
		break;
	/****STA******/
	case 0x85: //STA 0x2B �ۼ���A���ڴ� ��ҳ 2�ֽ�
		this->STA(M_ZERO);
		ADD_CYCLE(3);
		break;
	case 0x95: //STA 0x2B,X ��ҳX 2�ֽ�
		this->STA(M_X_ZERO);
		ADD_CYCLE(4);
		break;
	case 0x8D: {//STA 0x002B ���� 3�ֽ�
		CString xs;
		xs.Format(L"8D p:%d", this->pause);
		//MessageBox(NULL, xs, L"t", MB_OK);
		this->STA(M_ABS);
		ADD_CYCLE(4);
		break; }
	case 0x9D: //STA 0x002B,X ����X 3�ֽ�
		this->STA(M_X_ABS);
		ADD_CYCLE(5);
		break;
	case 0x99: //STA 0x002B,Y ����Y 3�ֽ�
		this->STA(M_Y_ABS);
		ADD_CYCLE(5);
		break;
	case 0x81: //STA (0x2B,X) ��ַX��� 2�ֽ�
		this->STA(M_X_IDA);
		ADD_CYCLE(6);
		break;
	case 0x91: //STA (0x2B),Y ��ӱ�ַY 2�ֽ�
		this->STA(M_IDA_Y);
		ADD_CYCLE(6);
		break;
	/****STX******/
	case 0x86: //STX 0x2B �Ĵ���X���ڴ�  ��ҳ 2�ֽ�
		this->STX(M_ZERO);
		ADD_CYCLE(3);
		break;
	case 0x96: //STX 0x2B,Y ��ҳY 2�ֽ�
		this->STX(M_Y_ZERO);
		ADD_CYCLE(4);
		break;
	case 0x8E: //STX 0x002B ���� 3�ֽ�
		this->STX(M_ABS);
		ADD_CYCLE(4);
		break;
	/****STY******/
	case 0x84: //STY 0x2B �Ĵ���X���ڴ�  ��ҳ 2�ֽ�
		this->STY(M_ZERO);
		ADD_CYCLE(3);
		break;
	case 0x94: //STY 0x2B,X ��ҳX 2�ֽ�
		this->STY(M_X_ZERO);
		ADD_CYCLE(4);
		break;
	case 0x8C: //STY 0x002B ���� 3�ֽ�
		this->STY(M_ABS);
		ADD_CYCLE(4);
		break;
	/*******�Ĵ������Ĵ���********/
	case 0xAA: //TAX A->X 1�ֽ�
		this->TAX();
		ADD_CYCLE(2);
		break;
	case 0xA8: //TAY A->Y 1�ֽ�
		this->TAY();
		ADD_CYCLE(2);
		break;
	case 0xBA: //TSX S->X 1�ֽ�
		this->TSX();
		ADD_CYCLE(2);
		break;
	case 0x8A: //TXA X->A 1�ֽ�
		this->TXA();
		ADD_CYCLE(2);
		break;
	case 0x9A: //TXS X->S 1�ֽ�
		this->TXS();
		ADD_CYCLE(2);
		break;
	case 0x98: //TYA Y->A 1�ֽ�
		this->TYA();
		ADD_CYCLE(2);
		break;
	/*******��ջ����********/
	case 0x48: //PHA A��ջ 1�ֽ�
		this->PHA();
		ADD_CYCLE(3);
		break;
	case 0x08: //PHP P��ջ 1�ֽ�
		this->PHP();
		ADD_CYCLE(3);
		break;
	case 0x68: //PLA ��ջ��A 1�ֽ�
		this->PLA();
		ADD_CYCLE(4);
		break;
	case 0x28: //PLP ��ջ��P 1�ֽ�
		this->PLP();
		ADD_CYCLE(4);
		break;
	case 0xEA: //NOP
		this->NOP();
		ADD_CYCLE(2);
		break;
	case 0x24:
		this->BIT(M_ZERO);
		ADD_CYCLE(3);
		break;
	case 0x2C:
		this->BIT(M_ABS);
		ADD_CYCLE(4);
		break;
	default:
		sprintf(asm_str, "ָ�����");
		break;
	}
	//MessageBox(NULL, L"OP CODE END.", L"t", MB_OK);
	this->printAsm();
	R.PC += opsize;
	return CERR;
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
		sprintf(str, "0x%04X,X[%X]", addr, MEM[addr + R.X]);
		sprintf(hstr, "%02X%02X", opd, opd2);
		addr += R.X;
		opsize = 3;
		break;
	case M_Y_ABS:
		//��һ���ֽ�Ϊ��8λ���ڶ���Ϊ��8λ
		addr  = opd | (opd2 << 8);;
		sprintf(str, "0x%04X,Y[%X]", addr, MEM[addr + R.Y]);
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
		sprintf(str, "%02X", opd);
		sprintf(hstr, "%02X", opd);
		opsize = 2;
		break;
	case M_Y_ZERO:
		addr = opd + R.Y;
		sprintf(str, "%02X", opd);
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
		sprintf(str, "(0x%02X),Y", opd);
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
		//MessageBox(NULL, L"OP SUCCES", L"t", MB_OK);
		//��ַ0x2000-0x2007ΪPPU�Ĵ���
		if (addr >= 0x2000 && addr <= 0x2007) {
			//MessageBox(NULL, L"CAO", L"t", MB_OK);
			v = 0;
			if (asm_str[0] == 'L') {
				//MessageBox(NULL, L"read reg ppu", L"title", MB_OK);
				v = g_PPU.readREG(addr & 0x07);
			}
		}
		else if (addr == 0x4000) {

		}
		else {
			//CPU�ڴ��
			v = MEM[addr];
		}
		if (paddr != NULL)
			*paddr = addr;
	}
	else {
		//MessageBox(NULL, L"OP ERROR", L"t", MB_OK);
	}
	dumpError();
	return v;
}

//��ȡ
byte CPU::read(word addr) {
	//��ַ0x2000-0x2007ΪPPU�Ĵ���
	if (addr >= 0x2000 && addr <= 0x2007) {
		//MessageBox(NULL, L"PPU READ", L"title", MB_OK);
		return g_PPU.readREG(addr & 0x07);
	}
	else if (addr == 0x4010) {
		return 0;
	}
	else {
		return MEM[addr & 0xffff];
	}
}

//���ַд��ֵ
void CPU::write(word addr, byte value) {
	//��ַ0x2000-0x2007ΪPPU�Ĵ���
	if (addr >= 0x2000 && addr <= 0x2007) {
		CString t;
		
		g_PPU.writeREG(addr & 0x07, value);
		t.Format(L"WRITE:%d,p:%d,pause��ַ:%X, REG��ַ:%X, CPU:%X=PPU:%X, REG6_ADDR:%X, REG7_INC:%X, opnum:%X", value, pause, &pause, &g_PPU.REG[0],
		    this, &g_PPU, &g_PPU.REG6_ADDR, &g_PPU.REG7_INC, &opnum);
		if (addr == 0x2006 && value == 0x28) {
			t.Format(L"asm str:%s", asm_str);
			//MessageBox(NULL, t, L"title", MB_OK);
		}
		//MessageBox(NULL, t, L"title", MB_OK);
	}
	else if (addr == 0x4014) { //DMA��ʽ���Ƶ�����RAM
		g_PPU.dmaSRAM(&MEM[(value * 0x100) & 0xffff]);
	}
	else {
		MEM[addr] = value;
	}
}
// NMI�ж� ������VBlank�ڼ�
void CPU::NMI() {
	//R.PC += 2; //��ָ��1���ֽ� +1����һ��ָ��
	PUSH(R.PC >> 8); //��λ����ջ 
	PUSH(R.PC & 0xff); //��λ����ջ
	PUSH(R.P);
	SET_FLAG(I_FLAG);
	R.PC = MEM[NMI_VECTOR] | (MEM[NMI_VECTOR + 1] << 8);
}
/* ADC (NV----ZC) */
void CPU::ADC(CPU6502_MODE mode) {
	this->setAsmOpStr("ADC");
	DT = this->value(mode); //���Ҫ�ۼӵ�ֵ
	if (CPUSUC(err.code)) { //ָ����Ч
		//A + M + C -> A
		sprintf(remark, "�Ĵ���A+0x%02x+0x%02x", DT, R.P & C_FLAG);
		WT = R.A + DT + (R.P & C_FLAG);				
		TST_FLAG(WT > 0xFF, C_FLAG);				
		TST_FLAG(((~(R.A^DT))&(R.A^WT) & 0x80), V_FLAG);
		//����ӽ���Ż��ۼ���A
		R.A = (byte)WT;
		SET_ZN_FLAG(R.A);
	}
}
/* INC (N-----Z-) */
void CPU::INC(CPU6502_MODE mode) {
	this->setAsmOpStr("INC");
	word addr;
	DT = this->value(mode, &addr); //���Ҫ�ۼӵ�ֵ
	if (CPUSUC(err.code)) { //ָ����Ч
		//M + 1 -> M
		sprintf(remark, "0x%02X+1", DT);
		DT++;
		//���д���ַ��
		this->write(addr, (byte)DT);
		SET_ZN_FLAG(DT);
	}
}
/* INX (N-----Z-) */
void CPU::INX() {
	this->setAsmOpStr("INX");
	sprintf(remark, "�Ĵ���X+1");
	if (g_PPU.REG6_ADDR) {
		CString ts;
		ts.Format(L"INX:%X", g_PPU.REG6_ADDR);
		//MessageBox(NULL, ts, L"t", MB_OK);
	}
	//X + 1 -> X
	R.X++;
	SET_ZN_FLAG(R.X);
	opsize = 1;
}
/* INY (N-----Z-) */
void CPU::INY() {
	this->setAsmOpStr("INY");
	sprintf(remark, "�Ĵ���Y+1");
	//Y + 1 -> Y
	R.Y++;
	SET_ZN_FLAG(R.Y);
	opsize = 1;
}
/* SBC (NV----ZC) */
void CPU::SBC(CPU6502_MODE mode) {
	this->setAsmOpStr("SBC");
	DT = this->value(mode);
	sprintf(remark, "�Ĵ���A-0x%02x-0x%02x", DT, (~R.P) & C_FLAG);
	//��Ҫ��C��λ
	WT = R.A - DT - ((~R.P) & C_FLAG);
	TST_FLAG(((R.A^DT) & (R.A^WT) & 0x80), V_FLAG);
	TST_FLAG(WT < 0x100, C_FLAG);
	R.A = (byte)WT;
	SET_ZN_FLAG(R.A);
}
/* DEX (N-----Z-) */
void CPU::DEC(CPU6502_MODE mode) {
	this->setAsmOpStr("DEC");
	word addr;
	DT = this->value(mode, &addr); //���Ҫ����ֵ
	if (CPUSUC(err.code)) { //ָ����Ч
		//M - 1 -> M
		sprintf(remark, "%X-1", DT);
		DT--;
		//���д���ַ��
		this->write(addr, (byte)DT);
		SET_ZN_FLAG(DT);
	}
}
/* DEX (N-----Z-) */
void CPU::DEX() {
	this->setAsmOpStr("DEX");
	sprintf(remark, "�Ĵ���X-1");
	R.X--;
	SET_ZN_FLAG(R.X);
	opsize = 1;
}
/* DEX (N-----Z-) */
void CPU::DEY() {
	this->setAsmOpStr("DEY");
	sprintf(remark, "�Ĵ���Y-1");
	R.Y--;
	SET_ZN_FLAG(R.Y);
	opsize = 1;
}
/* AND (N-----Z-) */
void CPU::AND(CPU6502_MODE mode) {
	this->setAsmOpStr("AND");
	DT = this->value(mode);
	sprintf(remark, "�Ĵ���A&0x%02X", DT);
	R.A &= (byte)DT;
	SET_ZN_FLAG(R.A);
}
/* EOR (N-----Z-) */
void CPU::EOR(CPU6502_MODE mode) {
	this->setAsmOpStr("EOR");
	DT = this->value(mode);
	sprintf(remark, "�Ĵ���A^0x%02X", DT);
	R.A ^= (byte)DT;
	SET_ZN_FLAG(R.A);
}
/* ORA (N-----Z-) */
void CPU::ORA(CPU6502_MODE mode) {
	this->setAsmOpStr("ORA");
	DT = this->value(mode);
	sprintf(remark, "�Ĵ���A|%X", DT);
	R.A |= (byte)DT;
	SET_ZN_FLAG(R.A);
}
/* ASL (N-----ZC) */
void CPU::ASL(CPU6502_MODE mode) {
	this->setAsmOpStr("ASL");
	if (mode == M_A) { //�ۼ���A����
		sprintf(remark, "�Ĵ���A����һλ");
		TST_FLAG(R.A & 0x80, C_FLAG);
		R.A <<= 1;
		SET_ZN_FLAG(R.A);
		opsize = 1;
	}
	else {
		word addr;
		DT = this->value(mode, &addr);
		sprintf(remark, "0x%02x����һλ", DT);
		TST_FLAG(DT & 0x80, C_FLAG);
		DT <<= 1;
		this->write(addr, (byte)DT);
		SET_ZN_FLAG(DT);
	}
}
/* LSR_A (N-----ZC) */
void CPU::LSR(CPU6502_MODE mode) {
	this->setAsmOpStr("LSR");
	if (mode == M_A) { //�ۼ���A����
		sprintf(remark, "�Ĵ���A����һλ");
		TST_FLAG(R.A & 0x01, C_FLAG);
		R.A >>= 1;
		SET_ZN_FLAG(R.A);
		opsize = 1;
	}
	else {
		word addr;
		DT = this->value(mode, &addr);
		sprintf(remark, "0x%02x����һλ", DT);
		TST_FLAG(DT & 0x01, C_FLAG);
		DT >>= 1;
		this->write(addr, (byte)DT);
		SET_ZN_FLAG(DT);
	}
}
/* ROL (N-----ZC) */
void CPU::ROL(CPU6502_MODE mode) {
	this->setAsmOpStr("ROL");
	if (mode == M_A) { //�ۼ���A����
		sprintf(remark, "�Ĵ���Aѭ������һλ");
		if (R.P & C_FLAG) { //CλҲ������λ
			TST_FLAG(R.A & 0x80, C_FLAG);
			R.A = (R.A << 1) | 0x01;
		}
		else {
			TST_FLAG(R.A & 0x80, C_FLAG);
			R.A <<= 1;
		}
		SET_ZN_FLAG(R.A);
		opsize = 1;
	}
	else {
		word addr;
		DT = this->value(mode, &addr);
		sprintf(remark, "0x%02xѭ������һλ", DT);
		if (R.P & C_FLAG) { //CλҲ������λ �����λ
			TST_FLAG(DT & 0x80, C_FLAG);
			DT = (DT << 1) | 0x01;
		}
		else {
			TST_FLAG(R.A & 0x80, C_FLAG);
			DT <<= 1;;
		}
		this->write(addr, (byte)DT);
		SET_ZN_FLAG(DT);
	}
}
/* ROR (N-----ZC) */
void CPU::ROR(CPU6502_MODE mode) {
	this->setAsmOpStr("ROR");
	if (mode == M_A) { //�ۼ���A����
		sprintf(remark, "�Ĵ���Aѭ������һλ");
		if (R.P & C_FLAG) { //CλҲ������λ
			TST_FLAG(R.A & 0x01, C_FLAG);
			R.A = (R.A >> 1) | 0x80;
		}
		else {
			TST_FLAG(R.A & 0x01, C_FLAG);
			R.A >>= 1;
		}
		SET_ZN_FLAG(R.A);
		opsize = 1;
	}
	else {
		word addr;
		DT = this->value(mode, &addr);
		sprintf(remark, "0x%02xѭ������һλ", DT);
		if (R.P & C_FLAG) { //CλҲ������λ �����λ
			TST_FLAG(DT & 0x01, C_FLAG);
			DT = (DT >> 1) | 0x80;
		}
		else {
			TST_FLAG(R.A & 0x01, C_FLAG);
			DT >>= 1;;
		}
		this->write(addr, (byte)DT);
		SET_ZN_FLAG(DT);
	}
}
/* BIT (NV----Z-) */
void CPU::BIT(CPU6502_MODE mode) {
	this->setAsmOpStr("BIT");
	DT = this->value(mode);
	sprintf(remark, "BIT-0x%02x", DT);
	TST_FLAG((DT&R.A) == 0, Z_FLAG);
	TST_FLAG(DT & 0x80, N_FLAG);
	TST_FLAG(DT & 0x40, V_FLAG);
}
// ��������ת
void CPU::JMP(CPU6502_MODE mode) {
	this->setAsmOpStr("JMP");
	sprintf(remark, "��ת");
	word addr;
	word v = this->value(mode, &addr);
	R.PC = addr;
	opsize = 0;
}
// ��ת���ӳ���
void CPU::JSR() {
	this->setAsmOpStr("JSR");
	sprintf(remark, "��ת���ӳ���");
	word addr;
	DT = this->value(M_ABS, &addr);
	R.PC += 3; //��ָ��3���ֽ� +3����һ��ָ��
	PUSH(R.PC >> 8); //��λ����ջ 
	PUSH(R.PC & 0xff); //��λ����ջ
	R.PC = addr; //�ӳ����ַ
	opsize = 0;
}
// �����ӳ���
void CPU::RTS() {
	this->setAsmOpStr("RTS");
	R.PC  = POP(); //��λ
	R.PC |= POP() * 0x100; //��λ
	sprintf(remark, "�ӳ��򷵻�:%x,%x", R.PC);
	opsize = 0;
}
// ǿ�ƽ����ж� ��������
void CPU::BRK() {
	this->setAsmOpStr("BRK");
	sprintf(remark, "�����ж�");
	R.PC++; //��ָ��1���ֽ� +1����һ��ָ��
	PUSH(R.PC >> 8); //��λ����ջ 
	PUSH(R.PC & 0xff); //��λ����ջ
	SET_FLAG(B_FLAG);
	PUSH(R.P);
	SET_FLAG(I_FLAG);
	R.PC = MEM[IRQ_VECTOR] | (MEM[IRQ_VECTOR + 1] << 8);
}
// �жϷ���
void CPU::RTI() {
	this->setAsmOpStr("RTI");
	sprintf(remark, "�жϷ���");
	R.P = POP() | R_FLAG;
	R.PC = POP(); //��λ
	R.PC |= POP() * 0x100; //��λ
	opsize = 0;
}
/* CMP (N-----ZC) */
void CPU::CMP(CPU6502_MODE mode) {
	this->setAsmOpStr("CMP");
	DT = this->value(mode);
	WT = (word)R.A - (word)DT;
	sprintf(remark, "�Ƚ� 0x%02x-0x%02x", R.A, DT);
	TST_FLAG((WT & 0x8000) == 0, C_FLAG);
	SET_ZN_FLAG((byte)WT);
}
/* CPX (N-----ZC) */
void CPU::CPX(CPU6502_MODE mode) {
	this->setAsmOpStr("CPX");
	DT = this->value(mode);
	WT = (word)R.X - (word)DT;
	sprintf(remark, "�Ƚ� 0x%02x-0x%02x", R.X, DT);
	TST_FLAG((WT & 0x8000) == 0, C_FLAG);
	SET_ZN_FLAG((byte)WT);
}
/* CPY (N-----ZC) */
void CPU::CPY(CPU6502_MODE mode) {
	this->setAsmOpStr("CPY");
	DT = this->value(mode);
	WT = (word)R.Y - (word)DT;
	sprintf(remark, "�Ƚ� 0x%02x-0x%02x", R.Y, DT);
	TST_FLAG((WT & 0x8000) == 0, C_FLAG);
	SET_ZN_FLAG((byte)WT);
}
/* LDA (N-----Z-) */
void CPU::LDA(CPU6502_MODE mode) {
	this->setAsmOpStr("LDA");
	DT = this->value(mode);
	sprintf(remark, "װ�ص��Ĵ���A(%02X)", (byte)DT);
	//MessageBox(NULL, L"LDA", L"t", MB_OK);
	if (CPUSUC(err.code)) { //ָ����Ч
		//M -> A
		CString t;
		t.Format(L"DT:%d", DT);
		//::MessageBoxW(NULL, t, L"Title", MB_OK);
		R.A = (byte)DT;
		SET_ZN_FLAG(R.A);
	}
}
/* LDX (N-----Z-) */
void CPU::LDX(CPU6502_MODE mode) {
	this->setAsmOpStr("LDX");
	sprintf(remark, "װ�ص��Ĵ���X");
	DT = this->value(mode);
	if (CPUSUC(err.code)) { //ָ����Ч
		//M -> X
		R.X = (byte)DT;
		SET_ZN_FLAG(R.X);
	}
}
/* LDY (N-----Z-) */
void CPU::LDY(CPU6502_MODE mode) {
	this->setAsmOpStr("LDY");
	sprintf(remark, "װ�ص��Ĵ���Y");
	DT = this->value(mode);
	if (CPUSUC(err.code)) { //ָ����Ч
		//M -> Y
		R.Y = (byte)DT;
		SET_ZN_FLAG(R.Y);
	}
}
/* STA (--------) */
void CPU::STA(CPU6502_MODE mode) {
	this->setAsmOpStr("STA");
	word addr;
	CString xs;
	xs.Format(L"STA 1 p:%d", this->pause, asm_str);
	//MessageBox(NULL, xs, L"t", MB_OK);
	DT = this->value(mode, &addr);
	sprintf(remark, "�Ĵ���A(%02X)����ַ0x%04x", R.A, addr);
	xs.Format(L"STA 2 p:%d", this->pause, asm_str);
	//MessageBox(NULL, xs, L"t", MB_OK);
	this->write(addr, R.A);
	xs.Format(L"STA 3 p:%d", this->pause, asm_str);
	//MessageBox(NULL, xs, L"t", MB_OK);
}
/* STX (--------) */
void CPU::STX(CPU6502_MODE mode) {
	this->setAsmOpStr("STX");
	word addr;
	DT = this->value(mode, &addr);
	sprintf(remark, "�Ĵ���X->��ַ0x%04x", addr);
	this->write(addr, R.X);
}
/* STY (--------) */
void CPU::STY(CPU6502_MODE mode) {
	this->setAsmOpStr("STY");
	word addr;
	DT = this->value(mode, &addr);
	sprintf(remark, "�Ĵ���Y->��ַ0x%04x", addr);
	this->write(addr, R.Y);
}
/* TAX (N-----Z-) */
void CPU::TAX() {
	this->setAsmOpStr("TAX");
	sprintf(remark, "�Ĵ���A->X");
	R.X = R.A;
	SET_ZN_FLAG(R.X);
	opsize = 1;
}
/* TAY (N-----Z-) */
void CPU::TAY() {
	this->setAsmOpStr("TAY");
	sprintf(remark, "�Ĵ���A->Y");
	R.Y = R.A;
	SET_ZN_FLAG(R.Y);
	opsize = 1;
}
/* TSX (N-----Z-) */
void CPU::TSX() {
	this->setAsmOpStr("TSX");
	sprintf(remark, "�Ĵ���S->X");
	R.X = R.S;
	SET_ZN_FLAG(R.X);
	opsize = 1;
}
/* TXA (N-----Z-) */
void CPU::TXA() {
	this->setAsmOpStr("TXA");
	sprintf(remark, "�Ĵ���X->A");
	R.A = R.X;
	SET_ZN_FLAG(R.A);
	opsize = 1;
}
/* TXS (N-----Z-) */
void CPU::TXS() {
	this->setAsmOpStr("TXS");
	sprintf(remark, "�Ĵ���X->S");
	R.S = R.X;
	SET_ZN_FLAG(R.S);
	opsize = 1;
}
/* TYA (N-----Z-) */
void CPU::TYA() {
	this->setAsmOpStr("TYA");
	sprintf(remark, "�Ĵ���Y->A");
	R.A = R.Y;
	SET_ZN_FLAG(R.A);
	opsize = 1;
}
//CλΪ0 ��ת
void CPU::BCC() {
	this->setAsmOpStr("BCC");
	sprintf(remark, "��ת CλΪ0");
	this->BJMP(!(R.P & C_FLAG));
	opsize = 2;
}
//CλΪ1 ��ת
void CPU::BCS() {
	this->setAsmOpStr("BCS");
	sprintf(remark, "��ת CλΪ1");
	this->BJMP((R.P & C_FLAG));
	opsize = 2;
}
//ZλΪ0 ��ת
void CPU::BNE() {
	this->setAsmOpStr("BNE");
	sprintf(remark, "��ת ZλΪ0");
	this->BJMP(!(R.P & Z_FLAG));
}
//ZλΪ1 ��ת
void CPU::BEQ() {
	this->setAsmOpStr("BEQ");
	sprintf(remark, "��ת ZλΪ1");
	this->BJMP((R.P & Z_FLAG));
}
//NλΪ0 ��ת
void CPU::BPL() {
	this->setAsmOpStr("BPL");
	sprintf(remark, "��ת NλΪ0");
	this->BJMP(!(R.P & N_FLAG));
}
//NλΪ1 ��ת
void CPU::BMI() {
	this->setAsmOpStr("BMI");
	sprintf(remark, "��ת NλΪ1");
	this->BJMP((R.P & N_FLAG));
}
//VλΪ0 ��ת
void CPU::BVC() {
	this->setAsmOpStr("BVC");
	sprintf(remark, "��ת VλΪ0");
	this->BJMP(!(R.P & V_FLAG));
}
//vλΪ1 ��ת
void CPU::BVS() {
	this->setAsmOpStr("BVS");
	sprintf(remark, "��ת VλΪ1");
	this->BJMP((R.P & V_FLAG));
}
//������ת
void CPU::BJMP(bool JMP) {
	word addr;
	DT = this->value(M_OFT, &addr);
	word a = R.PC;
	if (JMP) {
		R.PC = R.PC + (sbyte)addr;
	}
}
// PHA A��ջ
void CPU::PHA() {
	this->setAsmOpStr("PHA");
	sprintf(remark, "�Ĵ���A��ջ");
	PUSH(R.A);
	opsize = 1;
}
// PHP P��ջ
void CPU::PHP() {
	this->setAsmOpStr("PHP");
	sprintf(remark, "��־��P��ջ");
	PUSH(R.P | B_FLAG);
	opsize = 1;
}
// PLA (N-----Z-) ��ջ��A
void CPU::PLA() {
	this->setAsmOpStr("PLA");
	sprintf(remark, "��ջ���Ĵ���A");
	R.A = POP();
	SET_ZN_FLAG(R.A);
	opsize = 1;
}
// PLP ��ջ��P
void CPU::PLP() {
	this->setAsmOpStr("PLP");
	sprintf(remark, "��ջ����־��P");
	R.P = POP() | R_FLAG;
	opsize = 1;
}
void CPU::NOP() {
	this->setAsmOpStr("NOP");
	sprintf(remark, "��ת");
	opsize = 1;
}
//����C��λ
void CPU::SEC() { 
	R.P |= 0x01;
	this->setAsmOpStr("SEC");
	sprintf(remark, "����Cλ ��λλ");
	opsize = 1;
}
//����Zλ ���Ϊ0
void CPU::SEZ() { 
	R.P |= 0x02;
}
//�����ж�λ[��ֹ�ж�]
void CPU::SEI() { 
	R.P |= 0x04; 
	this->setAsmOpStr("SEI");
	sprintf(remark, "����Iλ �ж�λ");
	opsize = 1;
}
//����ʮ����λ
void CPU::SED() { 
	R.P |= 0x08;
	this->setAsmOpStr("SED");
	sprintf(remark, "����Dλ ʮ����λ");
	opsize = 1;
}
//�������λ
void CPU::SEV() { R.P |= 0x40; }
//���ø���λ
void CPU::SEN() { R.P |= 0x80; }

//���C��λ
void CPU::CLC() { 
	R.P &= 0xfe;
	this->setAsmOpStr("CLC");
	sprintf(remark, "���Cλ ��λλ");
	opsize = 1;
}
//���Zλ�����Ϊ0
void CPU::CLZ() { R.P &= 0xfd; }
//����ж�λ[�����ж�]
void CPU::CLI() { 
	R.P &= 0xfb;
	this->setAsmOpStr("CLI");
	sprintf(remark, "���Iλ �ж�λ");
	opsize = 1;
}
//���ʮ����λ
void CPU::CLD() { 
	R.P &= 0xf7;
	this->setAsmOpStr("CLD");
	sprintf(remark, "���Dλ ʮ����λ");
	opsize = 1;
}
//������λ
void CPU::CLV() { 
	R.P &= 0xbf; 
	this->setAsmOpStr("CLV");
	sprintf(remark, "���Vλ ���λ");
	opsize = 1;
}
//�������λ
void CPU::CLN() { R.P &= 0x7f; }

//���C��λ
byte CPU::GEC() { return R.P & 0x01; }
//���Zλ���
byte CPU::GEZ() { return (R.P >> 1) & 0x01; }
//����ж�λ
byte CPU::GEI() { return (R.P >> 2) & 0x01; }
//���ʮ����λ
byte CPU::GED() { return (R.P >> 3) & 0x01; }
//������λ
byte CPU::GEV() { return (R.P >> 6) & 0x01; }
//��ø���λ
byte CPU::GEN() { return (R.P >> 7) & 0x01; }

void CPU::setPause(bool r) {
	pause = r;
	if (!r) {

	}
}

void CPU::setStep(bool r) {
	step = r;
	lrow++;
	//printf("PC:0x%x\n", R.PC);
	if(step)
		opcode(MEM[R.PC]);
}

void CPU::setAsmOpStr(const char* str) {
	asm_str[0] = str[0];
	asm_str[1] = str[1];
	asm_str[2] = str[2];
	asm_str[3] = 0;
}

void CPU::printAsm() {
	int dim = exec_opnum - opnum;
	if (opnum > 9150) {
		CString rs;
		rs.Format(L"all num:%d, opnum:%d", exec_opnum, opnum);
		//SetWindowTextW(GetDlgItem(dbgdlg, 1010), rs);
	}
	if (dim <= 100 && clist) {
		CString ra, hs(hex_str), as(asm_str), rms(remark);
		ra.Format(L"%X:", run_addr);
		//as.Format(L"%s", "fuck");
		//::MessageBox(NULL, as, L"title", MB_OK);
		/*****�������б�*****/
		if (clist->GetItemCount() > 100) {
			clist->DeleteItem(0);
		}
		int row = clist->GetItemCount();
		clist->SetItemCount(100);
		clist->InsertItem(row, ra); //һ��Ҫ���������ú������ʾ
		clist->SetItemText(row, 1, hs);
		clist->SetItemText(row, 2, as);
		clist->SetItemText(row, 3, rms);
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