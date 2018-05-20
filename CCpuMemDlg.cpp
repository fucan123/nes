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
END_MESSAGE_MAP()

BOOL CCpuMemDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE;
}

void CCpuMemDlg::OnDestroy() {
	CDialog::OnDestroy();
	delete this;
}
