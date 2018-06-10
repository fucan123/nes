#include "stdafx.h"
#include "nes_mfc.h"
#include "CNesInfoDlg.h"

extern int CStringHexToInt(CString str);
extern CString* explode(wchar_t* separator, CString str, int* count);
extern ROM g_ROM;
extern CPU g_CPU;
extern PPU g_PPU;

CNesInfoDlg::CNesInfoDlg() : CDialogEx(IDD_DIALOG_NESINFO)
{
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

	NES_HEADER* h = g_ROM.getHeader();
	int mapper = (h->control1 >> 4) | (h->control2 & 0xf0);
	CString name(g_ROM.getFile());
	CString info;
	info.Format(
		L"�ļ�����: %s\r\n"\
		"�ļ��ַ�: %c%c%c%c(%02x%02X%02X%02X)\r\n"\
		"ROM-����: %d(%dKB)\r\n"\
		"VROM����: %d(%dKB)\r\n"\
		"�ܵ�����: %dKB\r\n"\
		"MAPPER��: %d"
		,
		name.GetBuffer(),
		h->id[0], h->id[1], h->id[2], h->id[3], h->id[0], h->id[1], h->id[2], h->id[3],
		h->rom_count, h->rom_count * 16, h->vrom_count, h->vrom_count * 8, h->rom_count * 16 + h->vrom_count * 8,
		mapper);
	CString info2 = L"\r\n��������: ";
	if (h->control1 & 0x01) {
		info2 += L"��ֱ����(1)";
	}
	else {
		info2 += L"ˮƽ����(0)";
	}

	info2 += L"\r\n��ؼ���: ";
	if (h->control1 & 0x02) {
		info2 += L"��(1)";
	}
	else {
		info2 += L"��(0)";
	}

	info2 += L"\r\n�Ƿ�����: ";
	if (h->control1 & 0x08) {
		info2 += L"��(1)";
	}
	else {
		info2 += L"��(0)";
	}

	info2 += L"\r\n-Trainer: ";
	if (h->control1 & 0x04) {
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

	GetDlgItem(IDC_STATIC_NESINFO)->SetWindowText(info+info2);

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