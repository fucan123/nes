#include "stdafx.h"
#include "nes_mfc.h"
#include "CPpuMemDlg.h"

extern int CStringHexToInt(CString str);
extern CString* explode(wchar_t* separator, CString str, int* count);
extern CPU g_CPU;
extern PPU g_PPU;

CPpuMemDlg::CPpuMemDlg() : CDialogEx(IDD_DIALOG_PPUMEM)
{
}

BOOL CPpuMemDlg::PreTranslateMessage(MSG* pMsg) {
	//回车键跳到OnOK函数处理
	if (0 && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	else return CDialog::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(CPpuMemDlg, CDialogEx)
END_MESSAGE_MAP()

BOOL CPpuMemDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE;
}

void CPpuMemDlg::OnDestroy() {
	CDialog::OnDestroy();
	delete this;
}
