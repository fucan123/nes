// ����Ӧ�ó��򡰹��ڡ��˵���� CDebugDlg �Ի���
#pragma once

#include "afxwin.h"
#include "afxcmn.h"

class CNesInfoDlg : public CDialogEx
{
public:
	CNesInfoDlg();
	// �Ի�������
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