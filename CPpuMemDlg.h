// ����Ӧ�ó��򡰹��ڡ��˵���� CDebugDlg �Ի���
#pragma once

#include "afxwin.h"
#include "afxcmn.h"

class CPpuMemDlg : public CDialogEx
{
public:
	CPpuMemDlg();

	// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);  // PreTranslateMessage����Ϣ���͸�TranslateMessage����֮ǰ�����õ�
	virtual BOOL OnInitDialog();
	virtual void OnDestroy();
protected:
	DECLARE_MESSAGE_MAP()
public:
};