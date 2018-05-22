#include "stdafx.h"
#include "nes_mfc.h"
#include "CPpuMemDlg.h"

extern int CStringHexToInt(CString str);
extern CString* explode(wchar_t* separator, CString str, int* count);
extern CPU g_CPU;
extern PPU g_PPU;

CPpuMemDlg::CPpuMemDlg() : CDialogEx(IDD_DIALOG_PPUMEM)
{
	thread_exit = 0;
}

BOOL CPpuMemDlg::PreTranslateMessage(MSG* pMsg) {
	//�س�������OnOK��������
	if (0 && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	else return CDialog::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(CPpuMemDlg, CDialogEx)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL CPpuMemDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	cFont.CreateFont(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH&FF_SWISS, L"Courier New");
	GetDlgItem(IDC_EDIT_PPUMEM)->SetFont(&cFont);

	thread_exit = 0;
	AfxBeginThread(ShowMEM, this);
	return TRUE;
}

void CPpuMemDlg::OnCancel() {
	thread_exit = 1;
	while (thread_exit != 2);
	DestroyWindow();
	delete this;
}

HBRUSH CPpuMemDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
	if (pWnd->GetDlgCtrlID() == IDC_EDIT_PPUMEM)
	{
		pDC->SetTextColor(RGB(0x33, 0x33, 0x33));
	}
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}

UINT CPpuMemDlg::ShowMEM(LPVOID param) {
	CPpuMemDlg* dlg = (CPpuMemDlg*)param;
	while (true) {
		if (dlg->thread_exit == 1) {
			dlg->thread_exit = 2;
			goto exit;
		}
		CString text = L"�����ڴ�\r\n";
		text += L"----   00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F\r\n";
		int i = 0;
		for (; i < 0xff; i += 16) {
			CString temp;
			temp.Format(L"%02X     %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X\r\n",
				i,
				g_PPU.SRAM[i], g_PPU.SRAM[i + 1], g_PPU.SRAM[i + 2], g_PPU.SRAM[i + 3], g_PPU.SRAM[i + 4],
				g_PPU.SRAM[i + 5], g_PPU.SRAM[i + 6], g_PPU.SRAM[i + 7], g_PPU.SRAM[i + 8], g_PPU.SRAM[i + 9],
				g_PPU.SRAM[i + 10], g_PPU.SRAM[i + 11], g_PPU.SRAM[i + 12], g_PPU.SRAM[i + 13], g_PPU.SRAM[i + 14],
				g_PPU.SRAM[i + 15]);
			text += temp;
		}
		text += L"��ɫ��\r\n";
		text += L"----   00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F\r\n";
		i = 0x3F00;
		CString temp;
		temp.Format(L"REG6 ADDR:%X\r\n", g_PPU.REG6_ADDR);
		text += temp;

		temp.Format(L"����   %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X\r\n",
			g_PPU.MEM[i], g_PPU.MEM[i + 1], g_PPU.MEM[i + 2], g_PPU.MEM[i + 3], g_PPU.MEM[i + 4],
			g_PPU.MEM[i + 5], g_PPU.MEM[i + 6], g_PPU.MEM[i + 7], g_PPU.MEM[i + 8], g_PPU.MEM[i + 9],
			g_PPU.MEM[i + 10], g_PPU.MEM[i + 11], g_PPU.MEM[i + 12], g_PPU.MEM[i + 13], g_PPU.MEM[i + 14],
			g_PPU.MEM[i + 15]);
		text += temp; 
		i = 0x3F10;
		temp.Format(L"����   %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X\r\n",
			g_PPU.MEM[i], g_PPU.MEM[i + 1], g_PPU.MEM[i + 2], g_PPU.MEM[i + 3], g_PPU.MEM[i + 4],
			g_PPU.MEM[i + 5], g_PPU.MEM[i + 6], g_PPU.MEM[i + 7], g_PPU.MEM[i + 8], g_PPU.MEM[i + 9],
			g_PPU.MEM[i + 10], g_PPU.MEM[i + 11], g_PPU.MEM[i + 12], g_PPU.MEM[i + 13], g_PPU.MEM[i + 14],
			g_PPU.MEM[i + 15]);
		text += temp;
		text += L"������/���Ա�\r\n";
		text += L"----   00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F\r\n";
		for (i = 0x2000; i < 0x27EF; i += 16) {
			CString temp;
			temp.Format(L"%04X   %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X\r\n",
				i,
				g_PPU.MEM[i], g_PPU.MEM[i + 1], g_PPU.MEM[i + 2], g_PPU.MEM[i + 3], g_PPU.MEM[i + 4],
				g_PPU.MEM[i + 5], g_PPU.MEM[i + 6], g_PPU.MEM[i + 7], g_PPU.MEM[i + 8], g_PPU.MEM[i + 9],
				g_PPU.MEM[i + 10], g_PPU.MEM[i + 11], g_PPU.MEM[i + 12], g_PPU.MEM[i + 13], g_PPU.MEM[i + 14],
				g_PPU.MEM[i + 15]);
			text += temp;
		}
		int nVertPos = dlg->GetDlgItem(IDC_EDIT_PPUMEM)->GetScrollPos(SB_VERT);
		
		//dlg->SendDlgItemMessage(IDC_EDIT_PPUMEM, WM_VSCROLL, 0, nVertPos);
		//dlg->GetDlgItem(IDC_EDIT1)->SetScrollPos(SB_VERT, 500);
		int start, end;
		CEdit* pedit = (CEdit*)(dlg->GetDlgItem(IDC_EDIT_PPUMEM));
		pedit->GetSel(start, end);
		int x = pedit->LineFromChar();
		pedit->SetWindowText(text);
		//pedit->SetSel(start, end);
		
		//pedit->GetLine;
		pedit->LineScroll(nVertPos);
		Sleep(500);
	}
exit:
	return 0;
}