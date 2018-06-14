#include "stdafx.h"
#include "nes_mfc.h"
#include "CPpuMemDlg.h"
#include "NES/NES.h"

extern int CStringHexToInt(CString str);
extern CString* explode(wchar_t* separator, CString str, int* count);
extern CPU g_CPU;
extern PPU g_PPU;

CPpuMemDlg::CPpuMemDlg(NES* p) : CDialogEx(IDD_DIALOG_PPUMEM)
{
	nes = p;
	thread_exit = 0;
}

BOOL CPpuMemDlg::PreTranslateMessage(MSG* pMsg) {
	//回车键跳到OnOK函数处理
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

	// TODO:  在此更改 DC 的任何特性
	if (pWnd->GetDlgCtrlID() == IDC_EDIT_PPUMEM)
	{
		pDC->SetTextColor(RGB(0x33, 0x33, 0x33));
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

UINT CPpuMemDlg::ShowMEM(LPVOID param) {
	CPpuMemDlg* dlg = (CPpuMemDlg*)param;
	NES* nes = dlg->nes;
	while (true) {
		if (dlg->thread_exit == 1) {
			dlg->thread_exit = 2;
			goto exit;
		}
		CString text = L"精灵内存\r\n";
		text += L"----   00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F\r\n";
		int i = 0;
		for (; i < 0xff; i += 16) {
			CString temp;
			temp.Format(L"%02X     %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X\r\n",
				i,
				nes->ppu->SRAM[i], nes->ppu->SRAM[i + 1], nes->ppu->SRAM[i + 2], nes->ppu->SRAM[i + 3], nes->ppu->SRAM[i + 4],
				nes->ppu->SRAM[i + 5], nes->ppu->SRAM[i + 6], nes->ppu->SRAM[i + 7], nes->ppu->SRAM[i + 8], nes->ppu->SRAM[i + 9],
				nes->ppu->SRAM[i + 10], nes->ppu->SRAM[i + 11], nes->ppu->SRAM[i + 12], nes->ppu->SRAM[i + 13], nes->ppu->SRAM[i + 14],
				nes->ppu->SRAM[i + 15]);
			text += temp;
		}

		CString tx;
		tx.Format(L"%X=%X\r\n", CHRRom, PPU_MEM_BANK[0]);
		text += tx;

		text += L"字模\r\n";
		text += L"----   00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F\r\n";
		for (i = 0x0000; i < 0x2000; i += 16) {
			CString temp;
			temp.Format(L"%04X   %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X\r\n",
				i,
				nes->ppu->Read(i), nes->ppu->Read(i + 1), nes->ppu->Read(i + 2), nes->ppu->Read(i + 3), nes->ppu->Read(i + 4),
				nes->ppu->Read(i + 5), nes->ppu->Read(i + 6), nes->ppu->Read(i + 7), nes->ppu->Read(i + 8), nes->ppu->Read(i + 9),
				nes->ppu->Read(i + 10), nes->ppu->Read(i + 11), nes->ppu->Read(i + 12), nes->ppu->Read(i + 13), nes->ppu->Read(i + 14),
				nes->ppu->Read(i + 15));
			text += temp;
		}
		
		CString temp;
		text += L"调色板\r\n";
		text += L"----   00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F\r\n";
		i = 0x3F00;
		temp.Format(L"NMI:%d, REG6 ADDR:%X, REG7_ADDR:%X, N_TABLE_V:%d, N_TABLE_INDEX:%d\r\nSCROLL_REG:%d-%d\r\nSPR_SIZE:%d\r\n", 
			nes->ppu->IS_NMI, nes->ppu->REG6_ADDR, nes->ppu->REG7_ADDR, nes->ppu->N_TABLE_V, nes->ppu->N_TABLE_INDEX, nes->ppu->SCROLL_REG[0], nes->ppu->SCROLL_REG[1],
			nes->ppu->SPR_SIZE);
		text += temp;

		text += L"寄存器\r\n";
		text += L"----   00  01  02  03  04  05  06  07\r\n";
		temp.Format(L"0:%02X, 1:%02X, 2:%02X, 3:%02X, 4:%02X, 5:%02X, 6:%02X, 7:%02X\r\n",
			nes->ppu->REG[0], nes->ppu->REG[1], nes->ppu->REG[2], nes->ppu->REG[3], nes->ppu->REG[4], nes->ppu->REG[5], nes->ppu->REG[6], nes->ppu->REG[7]);
		text += temp;

		temp.Format(L"背景   %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X\r\n",
			nes->ppu->Read(i), nes->ppu->Read(i + 1), nes->ppu->Read(i + 2), nes->ppu->Read(i + 3), nes->ppu->Read(i + 4),
			nes->ppu->Read(i + 5), nes->ppu->Read(i + 6), nes->ppu->Read(i + 7), nes->ppu->Read(i + 8), nes->ppu->Read(i + 9),
			nes->ppu->Read(i + 10), nes->ppu->Read(i + 11), nes->ppu->Read(i + 12), nes->ppu->Read(i + 13), nes->ppu->Read(i + 14),
			nes->ppu->Read(i + 15));
		text += temp; 
		i = 0x3F10;
		temp.Format(L"精灵   %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X\r\n",
			nes->ppu->Read(i), nes->ppu->Read(i + 1), nes->ppu->Read(i + 2), nes->ppu->Read(i + 3), nes->ppu->Read(i + 4),
			nes->ppu->Read(i + 5), nes->ppu->Read(i + 6), nes->ppu->Read(i + 7), nes->ppu->Read(i + 8), nes->ppu->Read(i + 9),
			nes->ppu->Read(i + 10), nes->ppu->Read(i + 11), nes->ppu->Read(i + 12), nes->ppu->Read(i + 13), nes->ppu->Read(i + 14),
			nes->ppu->Read(i + 15));
		text += temp;
		text += L"命名表/属性表\r\n";
		text += L"----   00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F\r\n";
		for (i = 0x2000; i < 0x27FF; i += 16) {
			CString temp;
			temp.Format(L"%04X   %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X\r\n",
				i,
				nes->ppu->Read(i), nes->ppu->Read(i + 1), nes->ppu->Read(i + 2), nes->ppu->Read(i + 3), nes->ppu->Read(i + 4),
				nes->ppu->Read(i + 5), nes->ppu->Read(i + 6), nes->ppu->Read(i + 7), nes->ppu->Read(i + 8), nes->ppu->Read(i + 9),
				nes->ppu->Read(i + 10), nes->ppu->Read(i + 11), nes->ppu->Read(i + 12), nes->ppu->Read(i + 13), nes->ppu->Read(i + 14),
				nes->ppu->Read(i + 15));
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
		Sleep(100);
	}
exit:
	return 0;
}
