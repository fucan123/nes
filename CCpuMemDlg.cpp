#include "stdafx.h"
#include "nes_mfc.h"
#include "CCpuMemDlg.h"

extern int CStringHexToInt(CString str);
extern CString* explode(wchar_t* separator, CString str, int* count);
extern CPU g_CPU;
extern PPU g_PPU;

CCpuMemDlg::CCpuMemDlg() : CDialogEx(IDD_DIALOG_CPUMEM)
{
}

BOOL CCpuMemDlg::PreTranslateMessage(MSG* pMsg) {
	//回车键跳到OnOK函数处理
	if (0 && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	else return CDialog::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(CCpuMemDlg, CDialogEx)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL CCpuMemDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	cFont.CreateFont(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH&FF_SWISS, L"Courier New");
	GetDlgItem(IDC_EDIT_CPUMEM)->SetFont(&cFont);

	thread_exit = 0;
	AfxBeginThread(ShowMEM, this);

	return TRUE;
}

void CCpuMemDlg::OnCancel() {
	thread_exit = 1;
	while (thread_exit != 2);
	DestroyWindow();
	delete this;
}

HBRUSH CCpuMemDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if (pWnd->GetDlgCtrlID() == IDC_EDIT_CPUMEM)
	{
		pDC->SetTextColor(RGB(0x33, 0x33, 0x33));
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

UINT CCpuMemDlg::ShowMEM(LPVOID param) {
	CCpuMemDlg* dlg = (CCpuMemDlg*)param;
	while (true) {
		if (dlg->thread_exit == 1) {
			dlg->thread_exit = 2;
			goto exit;
		}
		CString text = L"CPU内存\r\n";
		text += L"----   00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F\r\n";
		text += L"---------------------------------------------------------------------\r\n";
		int i = 0;
		for (; i < 0xffff; i += 16) {
			CString temp;
			temp.Format(L"%04X   %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X\r\n",
				i,
				g_CPU.MEM[i], g_CPU.MEM[i + 1], g_CPU.MEM[i + 2], g_CPU.MEM[i + 3], g_CPU.MEM[i + 4],
				g_CPU.MEM[i + 5], g_CPU.MEM[i + 6], g_CPU.MEM[i + 7], g_CPU.MEM[i + 8], g_CPU.MEM[i + 9],
				g_CPU.MEM[i + 10], g_CPU.MEM[i + 11], g_CPU.MEM[i + 12], g_CPU.MEM[i + 13], g_CPU.MEM[i + 14],
				g_CPU.MEM[i + 15]);
			text += temp;
		}
		
		int nVertPos = dlg->GetDlgItem(IDC_EDIT_CPUMEM)->GetScrollPos(SB_VERT);

		//dlg->SendDlgItemMessage(IDC_EDIT_PPUMEM, WM_VSCROLL, 0, nVertPos);
		//dlg->GetDlgItem(IDC_EDIT1)->SetScrollPos(SB_VERT, 500);
		int start, end;
		CEdit* pedit = (CEdit*)(dlg->GetDlgItem(IDC_EDIT_CPUMEM));
		pedit->GetSel(start, end);
		int x = pedit->LineFromChar();
		pedit->SetWindowText(text);
		//pedit->SetSel(start, end);

		//pedit->GetLine;
		pedit->LineScroll(nVertPos);
		Sleep(50);
	}
exit:
	return 0;
}

