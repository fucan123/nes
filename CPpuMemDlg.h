// ����Ӧ�ó��򡰹��ڡ��˵���� CDebugDlg �Ի���
#pragma once

#include "afxwin.h"
#include "afxcmn.h"

class CPpuMemDlg : public CDialogEx
{
public:
	CPpuMemDlg();
	int thread_exit;
	// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	CFont cFont;
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);  // PreTranslateMessage����Ϣ���͸�TranslateMessage����֮ǰ�����õ�
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	static UINT ShowMEM(LPVOID param);
};