// 用于应用程序“关于”菜单项的 CDebugDlg 对话框
#pragma once

#include "afxwin.h"
#include "afxcmn.h"
#include "cpu.h"

extern int CStringHexToInt(CString str);
extern CPU g_CPU;

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
};

CDebugDlg::CDebugDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

BOOL CDebugDlg::PreTranslateMessage(MSG* pMsg) {
	//回车键跳到OnOK函数处理
	if (0 && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	else return CDialog::PreTranslateMessage(pMsg);
}

void CDebugDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ASM, m_list);
}

BEGIN_MESSAGE_MAP(CDebugDlg, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT_DOP, &CDebugDlg::OnEnChangeEditDOP)
	ON_BN_CLICKED(IDC_BUTTON_STEP, &CDebugDlg::OnBnClickedButtonStep)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, &CDebugDlg::OnBnClickedButtonPause)
	ON_BN_CLICKED(IDC_BUTTON_RUN, &CDebugDlg::OnBnClickedButtonRun)
END_MESSAGE_MAP()

BOOL CDebugDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	DWORD dwStyle = m_list.GetExtendedStyle();
	dwStyle |= (LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_REPORT);
	m_list.SetExtendedStyle(dwStyle);
	m_list.InsertColumn(0, L"地址", LVCFMT_LEFT, 50);
	m_list.InsertColumn(1, _T("16进制码"), LVCFMT_LEFT, 80);
	m_list.InsertColumn(2, _T("指令"), LVCFMT_LEFT, 108);
	m_list.InsertColumn(3, _T("注释"), LVCFMT_LEFT, 168);

	g_CPU.dbgdlg = this->m_hWnd;
	g_CPU.clist = &m_list;
	g_CPU.reset();

	//MessageBox(L"INIT");
	return TRUE;
}

void CDebugDlg::OnEnChangeEditDOP()
{
	//GetDlgItem(IDC_EDIT_DOP)->SetWindowText(L"xx");
	
}
//接受回车键处理
void CDebugDlg::OnOK() {
	CString text;
	GetDlgItem(IDC_EDIT_DOP)->GetWindowText(text);
	int len = text.GetLength();
	if (len > 0) {
		CString op = text.Left(1);
		if (op == 'm' || op == 'M') {
			int pos = 1, index = 0;;
			CString addr[10];
			while (pos++ < len && index < 10) {
				CString tmp = text.Mid(pos, 1);
				while (tmp == ' ') {
					pos++;
					tmp = text.Mid(pos, 1);
				}
				if (tmp != ' ') {
					int tpos = text.Find(' ', pos);
					CString tp;
					tp.Format(L"%d, index:%d", tpos, index);
					if (tpos > -1) {
						addr[index] = text.Mid(pos, tpos - pos);
						pos = tpos;
					}
					else {
						addr[index] = text.Mid(pos, len - pos);
						break;
					}
					
				}
				index++;
			}
			CString str;
			for (int i = 0; i <= index; i++) {
				if (addr[index].GetLength() > 0) {
					int addr_i = CStringHexToInt(addr[i]);
					CString t;
					t.Format(L"0x%04X:0x%02X ", addr_i, g_CPU.mapv(addr_i));
					str += t;
				}
			}
			GetDlgItem(IDC_STATIC_OPR)->SetWindowText(str);
			GetDlgItem(IDC_EDIT_DOP)->SetWindowText(L"");
		}
	}
}

void CDebugDlg::OnBnClickedButtonStep()
{
	// TODO: 在此添加控件通知处理程序代码
	g_CPU.setStep(true);
}


void CDebugDlg::OnBnClickedButtonPause()
{
	g_CPU.setPause(true);
}


void CDebugDlg::OnBnClickedButtonRun()
{
	g_CPU.run();
}

void CDebugDlg::OnDestroy() {
	CDialog::OnDestroy();
	delete this;
}