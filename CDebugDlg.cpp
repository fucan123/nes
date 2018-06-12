#include "stdafx.h"
#include "nes_mfc.h"
#include "CDebugDlg.h"
#include "NES/NES.h"

extern int CStringHexToInt(CString str);
extern CString* explode(wchar_t* separator, CString str, int* count);
extern CPU g_CPU;
extern PPU g_PPU;

CDebugDlg::CDebugDlg(NES* p) : CDialogEx(IDD_ABOUTBOX)
{
	nes = p;
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
	ON_BN_CLICKED(IDC_BUTTON_PCP, &CDebugDlg::OnBnClickedButtonPcp)
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

	nes->cpu->dbgdlg = this->m_hWnd;
	nes->cpu->clist = &m_list;
	//nes->cpu->pause = true;
	//nes->cpu->reset();

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
					t.Format(L"0x%04X:0x%02X ", addr_i, nes->cpu->Read(addr_i));
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
					nes->cpu->R.A = value;
				}
				else if (arr[1] == 'x' || arr[1] == 'X') {
					nes->cpu->R.X = value;
				}
				else if (arr[1] == 'y' || arr[1] == 'Y') {
					nes->cpu->R.Y = value;
				}
				else if (arr[1] == 's' || arr[1] == 'S') {
					nes->cpu->R.S = value;
				}
				else if (arr[1] == 'p' || arr[1] == 'P') {
					nes->cpu->R.P = value;
				}
				else if (arr[1] == "pc" || arr[1] == "PC") {
					nes->cpu->R.PC = value;
				}
				else {
					int addr = CStringHexToInt(arr[1]);
					nes->cpu->write(addr, value);
					opr.Format(L"0x%04X:0x%02X", addr, value);
				}

				CString rs;
				rs.Format(L"CPU寄存器   A:%02X   X:%02X   Y:%02X   S:%02X   P:%02X     PC:%02X",
					nes->cpu->R.A, nes->cpu->R.X, nes->cpu->R.Y, nes->cpu->R.S, nes->cpu->R.P, nes->cpu->R.PC);
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
					str.Format(L"%ws:%02X%02X", arr[1].GetBuffer(), nes->cpu->MEM[addr + 1], nes->cpu->MEM[addr]);
					GetDlgItem(IDC_STATIC_OPR)->SetWindowText(str);
				}
				
			}
		}
		if (arr[0] == "asm") {
			if (arr[1] == '0') {
				nes->cpu->show_asm = false;
			}
			else {
				nes->cpu->show_asm = true;
			}
			GetDlgItem(IDC_STATIC_OPR)->SetWindowText(L"设置成功");
		}
		if (arr[0] == "bgi" && count >= 3) {
			GetDlgItem(IDC_STATIC_OPR)->SetWindowText(L"");
			int x = CStringHexToInt(arr[1]);
			int y = CStringHexToInt(arr[2]) - 1;
			int i = 256 * y * 4 + (x * 4);
			byte* img = nes->cpu->images;
			bool find = false;
			CString str;
			for (int j = 0; j < 8; j++) {
				if (img[i + 3] > 0) {
					str.Format(L"COLOR:#%X%X%X, ALPHA:%d", img[i + 2], img[i + 1], img[i], img[i + 3]);
					find = true;
					break;
				}
				i += 4;
			}
			if (!find) {
				i = 256 * y * 4 + (x * 4);
				str.Format(L"COLOR:#%X%X%X, ALPHA:%d", img[i + 2], img[i + 1], img[i], img[i + 3]);
			}
			GetDlgItem(IDC_STATIC_OPR)->SetWindowText(str);
		}
	}
}

void CDebugDlg::OnBnClickedButtonStep()
{
	// TODO: 在此添加控件通知处理程序代码
	nes->cpu->setStep(true);
}


void CDebugDlg::OnBnClickedButtonPause()
{
	nes->cpu->setPause(true);
}


void CDebugDlg::OnBnClickedButtonRun()

{
	nes->cpu->run(10);
}

void CDebugDlg::OnBnClickedButtonOpnum()
{
	CString text;
	GetDlgItem(IDC_EDIT_OPNUM)->GetWindowText(text);
	if (text.GetLength() > 0) {
		nes->cpu->setStep(false);
		nes->cpu->setPause(false);
		nes->cpu->opnum = 0;
		nes->cpu->exec_opnum = _ttoi(text);
		
		//nes->cpu->run(_ttoi(text));
		//nes->cpu->opnum = 0;
	}
}

void CDebugDlg::GetDispInfo_M(NMHDR* pNMHDR, LRESULT* pResult) {

}

void CDebugDlg::OnDestroy() {
	CDialog::OnDestroy();
	delete this;
}


void CDebugDlg::OnBnClickedButtonPcp()
{
	CString text;
	GetDlgItem(IDC_EDIT_PCP)->GetWindowText(text);
	if (text.GetLength() > 0) {
		nes->cpu->setStep(false);
		nes->cpu->setPause(false);
		nes->cpu->opnum = 0;
		nes->cpu->exec_opnum = 0x7fffffff;
		nes->cpu->pcp = CStringHexToInt(text);
		//nes->cpu->run(_ttoi(text));
		//nes->cpu->opnum = 0;
	}
}
