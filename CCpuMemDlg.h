// 用于应用程序“关于”菜单项的 CDebugDlg 对话框
#pragma once

#include "afxwin.h"
#include "afxcmn.h"

class CCpuMemDlg : public CDialogEx
{
public:
	CCpuMemDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);  // PreTranslateMessage是消息在送给TranslateMessage函数之前被调用的
	virtual BOOL OnInitDialog();
	virtual void OnDestroy();
protected:
	DECLARE_MESSAGE_MAP()
public:
};