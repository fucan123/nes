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
		L"文件名称: %s\r\n"\
		"文件字符: %c%c%c%c(%02x%02X%02X%02X)\r\n"\
		"ROM-数量: %d(%dKB)\r\n"\
		"VROM数量: %d(%dKB)\r\n"\
		"总的容量: %dKB\r\n"\
		"MAPPER号: %d"
		,
		name.GetBuffer(),
		h->id[0], h->id[1], h->id[2], h->id[3], h->id[0], h->id[1], h->id[2], h->id[3],
		h->rom_count, h->rom_count * 16, h->vrom_count, h->vrom_count * 8, h->rom_count * 16 + h->vrom_count * 8,
		mapper);
	CString info2 = L"\r\n镜像类型: ";
	if (h->control1 & 0x01) {
		info2 += L"垂直镜像(1)";
	}
	else {
		info2 += L"水平镜像(0)";
	}

	info2 += L"\r\n电池记忆: ";
	if (h->control1 & 0x02) {
		info2 += L"有(1)";
	}
	else {
		info2 += L"无(0)";
	}

	info2 += L"\r\n是否四屏: ";
	if (h->control1 & 0x08) {
		info2 += L"是(1)";
	}
	else {
		info2 += L"否(0)";
	}

	info2 += L"\r\n-Trainer: ";
	if (h->control1 & 0x04) {
		info2 += L"有(1)";
	}
	else {
		info2 += L"无(0)";
	}

	info2 += L"\r\n十六进制: ";
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

	// TODO:  在此更改 DC 的任何特性
	pDC->SetTextColor(RGB(0x33, 0x33, 0x33));
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}