#include "stdafx.h"
#include "nes_mfc.h"
#include "CDebugDlg.h"

extern int CStringHexToInt(CString str);
extern CString* explode(wchar_t* separator, CString str, int* count);
extern CPU g_CPU;
extern PPU g_PPU;

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
	ON_BN_CLICKED(IDC_BUTTON_OPNUM, &CDebugDlg::OnBnClickedButtonOpnum)
	ON_NOTIFY(LVN_GETDISPINFO, 3, &CDebugDlg::GetDispInfo_M)
END_MESSAGE_MAP()

BOOL CDebugDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	DWORD dwStyle = m_list.GetExtendedStyle();
	dwStyle |= (LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_REPORT );
	m_list.SetExtendedStyle(dwStyle);
	m_list.InsertColumn(0, L"地址", LVCFMT_LEFT, 50);
	m_list.InsertColumn(1, _T("16进制码"), LVCFMT_LEFT, 80);
	m_list.InsertColumn(2, _T("指令"), LVCFMT_LEFT, 120);
	m_list.InsertColumn(3, _T("注释"), LVCFMT_LEFT, 153);
	GetDlgItem(IDC_EDIT_OPNUM)->SetWindowText(L"10000");

	g_CPU.dbgdlg = this->m_hWnd;
	g_CPU.clist = &m_list;
	//g_CPU.pause = true;
	//g_CPU.reset();

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
		int count;
		CString* arr = explode(L" ", text, &count);
		if (arr[0] == 'm' || arr[0] == 'M') {
			CString str;
			for (int i = 1; i < count; i++) {
				if (arr[i].GetLength() > 0) {
					int addr_i = CStringHexToInt(arr[i]);
					CString t;
					t.Format(L"0x%04X:0x%02X ", addr_i, g_CPU.read(addr_i));
					str += t;
				}
			}
			GetDlgItem(IDC_STATIC_OPR)->SetWindowText(str);
			GetDlgItem(IDC_EDIT_DOP)->SetWindowText(L"");
		}
		if (arr[0] == 's' || arr[0] == 'S') {
			if (count >= 3) {
				int value = CStringHexToInt(arr[2]);
				CString opr;
				opr.Format(L"R.%ws:0x%04X", arr[1].GetBuffer(), value);
				if (arr[1] == 'a' || arr[1] == 'A') {
					g_CPU.R.A = value;
				}
				else if (arr[1] == 'x' || arr[1] == 'X') {
					g_CPU.R.X = value;
				}
				else if (arr[1] == 'y' || arr[1] == 'Y') {
					g_CPU.R.Y = value;
				}
				else if (arr[1] == 's' || arr[1] == 'S') {
					g_CPU.R.S = value;
				}
				else if (arr[1] == 'p' || arr[1] == 'P') {
					g_CPU.R.P = value;
				}
				else if (arr[1] == "pc" || arr[1] == "PC") {
					g_CPU.R.PC = value;
				}
				else {
					int addr = CStringHexToInt(arr[1]);
					g_CPU.write(addr, value);
					opr.Format(L"0x%04X:0x%02X", addr, value);
				}

				CString rs;
				rs.Format(L"CPU寄存器   A:%02X   X:%02X   Y:%02X   S:%02X   P:%02X     PC:%02X",
					g_CPU.R.A, g_CPU.R.X, g_CPU.R.Y, g_CPU.R.S, g_CPU.R.P, g_CPU.R.PC);
				GetDlgItem(IDC_STATIC_RGV)->SetWindowText(rs);
				GetDlgItem(IDC_STATIC_OPR)->SetWindowText(opr);
				//GetDlgItem(IDC_EDIT_DOP)->SetWindowText(arr[0]);
			}
		}
		if (arr[0] == 'i' || arr[0] == 'I') {
			if (count >= 2) {
				word addr = 0;
				if (arr[1] == "nmi" || arr[1] == "NMI") {
					addr = 0xFFFA;
				}
				if (arr[1] == "rest" || arr[1] == "rest") {
					addr = 0xFFFC;
				}
				if (arr[1] == "irq" || arr[1] == "IRQ") {
					addr = 0xFFFE;
				}
				if (addr == 0) {
					GetDlgItem(IDC_STATIC_OPR)->SetWindowText(L"无效指令");
				}
				else {
					CString str;
					str.Format(L"%ws:%02X%02X", arr[1].GetBuffer(), g_CPU.MEM[addr + 1], g_CPU.MEM[addr]);
					GetDlgItem(IDC_STATIC_OPR)->SetWindowText(str);
				}
				
			}
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
	g_CPU.run(10);
}

void CDebugDlg::OnBnClickedButtonOpnum()
{
	CString text;
	GetDlgItem(IDC_EDIT_OPNUM)->GetWindowText(text);
	if (text.GetLength() > 0) {
		g_CPU.setStep(false);
		g_CPU.setPause(false);
		g_CPU.opnum = 0;
		g_CPU.exec_opnum = _ttoi(text);
		
		//g_CPU.run(_ttoi(text));
		//g_CPU.opnum = 0;
	}
}

void CDebugDlg::GetDispInfo_M(NMHDR* pNMHDR, LRESULT* pResult) {

}

void CDebugDlg::OnDestroy() {
	CDialog::OnDestroy();
	delete this;
}
