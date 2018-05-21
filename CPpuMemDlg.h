// 用于应用程序“关于”菜单项的 CDebugDlg 对话框
#pragma once

#include "afxwin.h"
#include "afxcmn.h"

class CPpuMemDlg : public CDialogEx
{
public:
	CPpuMemDlg();
	int thread_exit;
	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	CFont cFont;
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);  // PreTranslateMessage是消息在送给TranslateMessage函数之前被调用的
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	static UINT ShowMEM(LPVOID param);
};