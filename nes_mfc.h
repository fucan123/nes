
// nes_mfc.h : nes_mfc 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号
#include "NES/ROM.h"
#include "NES/CPU.h"
#include "NES/PPU.h"

// Cnes_mfcApp:
// 有关此类的实现，请参阅 nes_mfc.cpp
//

class Cnes_mfcApp : public CWinApp
{
protected:
	void *dlgdbg;
	void *dlgcpumem;
	void *dlgppumem;
public:
	NES* nes;

	Cnes_mfcApp();
// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDebug();
	afx_msg void OnCPUMem();
	afx_msg void OnPPUMem();
	afx_msg void OnNesInfo();
};

extern Cnes_mfcApp theApp;
