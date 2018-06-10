// 用于应用程序“关于”菜单项的 CDebugDlg 对话框
#pragma once

#include "afxwin.h"
#include "afxcmn.h"

class CNesInfoDlg : public CDialogEx
{
public:
	CNesInfoDlg();
	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	CFont cFont;
protected:
	virtual BOOL OnInitDialog();
	virtual void OnDestroy();
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};