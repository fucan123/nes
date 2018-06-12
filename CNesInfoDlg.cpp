#include "stdafx.h"
#include "nes_mfc.h"
#include "CNesInfoDlg.h"
#include "NES/NES.h"

extern int CStringHexToInt(CString str);
extern CString* explode(wchar_t* separator, CString str, int* count);
extern ROM g_ROM;
extern CPU g_CPU;
extern PPU g_PPU;

CNesInfoDlg::CNesInfoDlg(NES* p) : CDialogEx(IDD_DIALOG_NESINFO)
{
	nes = p;
}

BEGIN_MESSAGE_MAP(CNesInfoDlg, CDialogEx)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL CNesInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	cFont.CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH&FF_SWISS, L"Courier New");
	GetDlgItem(IDC_STATIC_NESINFO)->SetFont(&cFont);

	NES_HEADER* h = nes->rom->GetHeader();
	int mapper = (h->Control1 >> 4) | (h->Control2 & 0xf0);
	CString name(nes->rom->GetFile());
	CString info;
	info.Format(
		L"�ļ�����: %s(%dKB)\r\n"\
		"�ļ��ַ�: %c%c%c%c(%02x%02X%02X%02X)\r\n"\
		"ROM-����: %d(%dKB)\r\n"\
		"VROM����: %d(%dKB)\r\n"\
		"�ܵ�����: %dKB\r\n"\
		"MAPPER��: %d"
		,
		name.GetBuffer(), nes->rom->filesize / 1024,
		h->ID[0], h->ID[1], h->ID[2], h->ID[3], h->ID[0], h->ID[1], h->ID[2], h->ID[3],
		h->PRG_PAGE_SIZE, h->PRG_PAGE_SIZE * 16, h->CHR_PAGE_Size, h->CHR_PAGE_Size * 8, h->PRG_PAGE_SIZE * 16 + h->CHR_PAGE_Size * 8,
		mapper);
	CString info2 = L"\r\n��������: ";
	if (h->Control1 & 0x01) {
		info2 += L"��ֱ����(1)";
	}
	else {
		info2 += L"ˮƽ����(0)";
	}

	info2 += L"\r\n��ؼ���: ";
	if (h->Control1 & 0x02) {
		info2 += L"��(1)";
	}
	else {
		info2 += L"��(0)";
	}

	info2 += L"\r\n�Ƿ�����: ";
	if (h->Control1 & 0x08) {
		info2 += L"��(1)";
	}
	else {
		info2 += L"��(0)";
	}

	info2 += L"\r\n-Trainer: ";
	if (h->Control1 & 0x04) {
		info2 += L"��(1)";
	}
	else {
		info2 += L"��(0)";
	}

	info2 += L"\r\nʮ������: ";
	BYTE* m = (BYTE*)h;
	for (int i = 0; i < 16; i++) {
		CString n;
		n.Format(L"%02X ", m[i]);
		info2 += n;
	}


	CString info3 = L"\r\n���ʮ��: ";
	int mi = nes->rom->filesize - 1 - 16;
	for (int i = 15; i >= 0; i--) {
		CString n;
		n.Format(L"%02X ", nes->rom->rom[mi - i]);
		info3 += n;
	}

	CString info4 = L"\r\n���ʮ��: ";
	mi = nes->rom->filesize - 1;
	for (int i = 15; i >= 0; i--) {
		CString n;
		n.Format(L"%02X ", nes->rom->rom[mi - i]);
		info4 += n;
	}

	GetDlgItem(IDC_STATIC_NESINFO)->SetWindowText(info+info2+info3+info4);

	return TRUE;
}

void CNesInfoDlg::OnDestroy() {
	CNesInfoDlg::OnDestroy();
}

HBRUSH CNesInfoDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
	pDC->SetTextColor(RGB(0x33, 0x33, 0x33));
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}