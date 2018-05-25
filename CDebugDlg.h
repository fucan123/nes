// 用于应用程序“关于”菜单项的 CDebugDlg 对话框
#pragma once

#include "afxwin.h"
#include "afxcmn.h"

class CDebugDlg : public CDialogEx
{
public:
	CDebugDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL PreTranslateMessage(MSG* pMsg);  // PreTranslateMessage是消息在送给TranslateMessage函数之前被调用的
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnDestroy();
protected:
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list;

	afx_msg void OnEnChangeEditDOP();
	afx_msg void OnBnClickedButtonStep();
	afx_msg void OnBnClickedButtonPause();
	afx_msg void OnBnClickedButtonRun();
	afx_msg void OnBnClickedButtonOpnum();
	afx_msg void GetDispInfo_M(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButtonPcp();
};