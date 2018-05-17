
// nes_mfc.h : nes_mfc 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号
#include "cpu.h"
#include "ppu.h"

// Cnes_mfcApp:
// 有关此类的实现，请参阅 nes_mfc.cpp
//

class Cnes_mfcApp : public CWinApp
{
protected:
	void *dlgdbg;
public:
	
	Cnes_mfcApp();
// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDebug();
	static UINT CPURun(LPVOID param);
	static UINT PPURun(LPVOID param);
};

extern Cnes_mfcApp theApp;
