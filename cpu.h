#pragma once

#include <stdio.h>
#include <iostream>
#include "ppu.h"

#ifndef CPUH
#define CPUH

#define DEBUGDLG(p) ((CDebugDlg*)p)

#define CPUSUC(code) (code == 0) 
#define CPUERR(code) (code > 0)
//结果是否为0
#define ISZ(v) (v == 0)
//结果是否需要进位
#define ISC(v) (v > 0xff)
//结果是否是负数
#define ISN(v) (v > 0x7f)
//结果是否溢出[最高位为1那么就为溢出]
#define ISV(v) (v > 0x7f)

typedef struct cpu6502 {
	byte A;  //8位累加寄存器
	byte X;  //8位变址寄存器X
	byte Y;  //8位变址寄存器Y
	/*
	8位标志寄存器
	0-C进位位:有进位为1，1-Z零标志位：执行结果为0此位为1，2-I中断标记位：为1禁止中断非屏蔽中断不受影响
	3-D位，4-B位，5-保留，
	6-V溢出标记位：若执行结果溢出此位为1，7-N负数标记位：若结果为负数此位为1↓
	*/
	byte P;  //8位标志寄存器
	byte S;  //8位堆栈指针，默认为0xff[桟段为0x100-0x1ff, 只存储了低字节, 需要加0x100], 进栈-1, 出栈+1
	word PC; //16位程序指令地址寄存器，存储了下一条指令执行的位置
} CPU6502;

enum CPU6502_CODE { //M表示动态取得的值
	CERR=-1,  //错误
	NOP = 0,  //空操作
	ADC = 1,  //带进位加法运算 A+M+C->A
	INC = 2,  //+1运算 M+1->M
	INX = 3,  //变址寄存器X+1 X+1->X
	INY = 4,  //变址寄存器Y+1 Y+1->Y
	SBC = 5,  //带借位减法 A-M-C->A
	DEC = 6,  //-1运算 M-1->M
	DEX = 7,  //变址寄存器X-1 X-1->X
	DEY = 8,  //变址寄存器Y-1 Y-1->Y
	AND = 9,  //逻辑与 A&M->A
	EOR = 10, //逻辑异或 A^M->A
	ORA = 11, //逻辑或 A|M->A
	BIT = 12, //位测试 A&M 结果第6位->V 7位->N
	ASL = 13, //左移一位 最高位进C标志位
	LSR = 14, //右移一位 最低位进C标记位
	ROL = 15, //左移一位 连同C标志位 既最高位进C标志位 C标志位进最低位
	ROR = 16, //右移一位 连同C标志位 既最低位进C标记位 C标记位进最高位
	CLC = 17, //进位标志C清0
	CLD = 18, //十进制位D清0
	CLI = 19, //中断位I清0
	CLV = 20, //溢出位清0
	SEC = 21, //设置进位C
	SED = 22, //设置十进制位D
	SEI = 23, //设置中断位I
	CMP = 24, //比较 A-M 等于0那么置Z位1，大于那么置C位1
	CPX = 25, //比较 X-M 等于0那么置Z位1，大于那么置C位1
	CPY = 26, //比较 Y-M 等于0那么置Z位1，大于那么置C位1
	LDA = 27, //送累加器A M->A
	LDX = 28, //送寄存器X M->X
	LDY = 29, //送寄存器Y M->Y
	STA = 31, //存累加器A A->M
	STX = 32, //存寄存器X X->M
	STY = 33, //存寄存器Y Y->M
	TAX = 34, //A->X
	TAY = 35, //A->Y
	TSX = 36, //S->X
	TXS = 37, //X->S
	TXA = 38, //X->A
	TYA = 39, //Y->A
	PHA = 40, //累加器A入栈
	PHP = 41, //标记寄存器P入栈
	PLA = 42, //出栈到累加器A
	PLP = 43, //出栈到标志寄存器P
	BEQ = 44, //标志Z位=1 转移
	BNE = 45, //标志Z位=0 转移
	BCS = 46, //标志C位=1 转移
	BCC = 47, //标志C位=0 转移
	BMI = 48, //标志N位=1 转移
	BPL = 49, //标志N位=0 转移
	BVS = 50, //标志V位=1 转移
	BVC = 51, //标志V位=0 转移
};

/*
寻址方式：
1.立即数，直接给出数字LDA #0xA 2字节
2.直接寻址[绝对寻址]，LDA 0x00A0 取0x00A0地址上的值 3字节
3.寄存器X直接变址[绝对变址] LDA 0x00A0,X addr=0x00A0+X 值=addr地址上的值 3字节
4.寄存器Y直接变址[绝对变址] LDA 0x00A0,Y addr=0x00A0+Y 值=addr地址上的值 3字节
5.零页寻址，LDA 0xA0 去地址0xA0上的值 零页范围(0x00-0xff) 2字节 [类似上面的寻址方式]
6.寄存器X零页变址  LDA 0xA0,X addr=0xA0+X 值=addr地址上的值 2字节
7.寄存器Y零页变址  LDA 0xA0,Y addr=0xA0+Y 值=addr地址上的值 2字节
8.间接寻址只用于JMP跳转指令 JMP 0x00A0 A1=0x00A0低8位0xCD A2=0x00A0+1地址值得到高8位)0xAB 值=0xABCD地址上的值 3字节
9.先变址X后间接寻址[只适用零页] LDA (0xA0,X) A1=0xA0+X地址值得到低8位0xCD A2=0xA0+X+1地址值得到高8位)0xAB 值=0xABCD地址上的值 2字节
10.间接后变址Y寻址[只适用零页] LDA (0xA0),Y A1=0xA0地址值得到低8位0xCD A2=0xA0+1地址值得到高8位)0xAB 值=0xABCD+Y地址上的值 2字节
*/
enum CPU6502_MODE {
	M_0      = 0,  //0
	M_Q      = 1,  //立即数
	M_ABS    = 2,  //绝对寻址
	M_X_ABS  = 3,  //寄存器X直接变址
	M_Y_ABS  = 4,  //寄存器Y直接变址
	M_ZERO   = 5,  //零页寻址
	M_X_ZERO = 6,  //寄存器X零页变址
	M_Y_ZERO = 7,  //寄存器Y零页变址
	M_IDA    = 8,  //间接寻址
	M_X_IDA  = 9,  //先变址X后间接寻址
	M_IDA_Y  = 10, //间接后变址Y寻址
	M_A      = 11, //累加器操作
	M_OFT    = 12, //偏移操作
};

enum CPU_ECODE {
	CPU_SUC    = 0, //没有错
	OP_ERR     = 1, //指令错误
};

class CPU {
public:
	CPU6502    R;
	word       DT;
	word       WT;
	byte       NF_TABLE[256];
	//指令执行周期
	int        EXEC_CYCLE;
	//所有执行周期
	int        TOTAL_CYCLE;
	//CPU内存映像, 只能寻址到0xffff
	byte       MEM[0xffff + 1];
	byte       PPU[0x7fff + 1];
	//桟段
	byte*      STACK;
	//指令长度
	byte       opsize;
	struct {
		CPU_ECODE code;
		byte      value;
		char*     err_str;
	}          err;
	//正在运行的地址
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
	
	void inline SEC(); //设置C进位
	void inline SEZ(); //设置Z位结果为0
	void inline SEI(); //设置中断位[禁止中断]
	void inline SED(); //设置十进制位
	void inline SEV(); //设置溢出位
	void inline SEN(); //设置负数位
	
	void inline CLC(); //清掉C进位
	void inline CLZ(); //清掉Z位结果不为0
	void inline CLI(); //清掉中断位[接受中断]
	void inline CLD(); //清掉十进制位
	void inline CLV(); //清掉溢出位
	void inline CLN(); //清掉负数位

	byte inline GEC(); //获得C进位
	byte inline GEZ(); //获得Z位结果不为0
	byte inline GEI(); //获得中断位[接受中断]
	byte inline GED(); //获得十进制位
	byte inline GEV(); //获得溢出位
	byte inline GEN(); //获得负数位


	void setPause(bool);
	void setStep(bool);
	void inline setAsmOpStr(const char*);
	void inline printAsm();
	void dumpError();
	void end() {
		//一条扫描线时间
		double line_time = 1 / 50 / 312; //每秒50帧 一帧312条扫描线
		int line = 0; //第几条扫描线
		LARGE_INTEGER freq, stime, ctime;
		QueryPerformanceFrequency(&freq); //获取时钟频率
		QueryPerformanceCounter(&stime); //113.6825
		while (true) {
			QueryPerformanceCounter(&ctime); //当前时间
			double dim = (double)(ctime.QuadPart - stime.QuadPart) / (double)freq.QuadPart;
			if (dim >= line_time) {
				//执行cpu指令 113.6825周期
				if (line < 240) {
					//绘制扫描线
				}
				else {
					//VBlank期间
				}
				if (++line == 312) //全部312扫描完成
					line = 0;
			}
		}

	}
	~CPU();
};
#endif