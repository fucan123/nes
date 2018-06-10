
// ChildView.cpp : CChildView ���ʵ��
//

#include "stdafx.h"
#include "nes_mfc.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//CPUȫ�ֱ���
extern CPU g_CPU;
//CPUȫ�ֱ���
extern PPU g_PPU;

// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
END_MESSAGE_MAP()



// CChildView ��Ϣ�������

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // ���ڻ��Ƶ��豸������
	GetClientRect(&rect);
	CString ts;
	ts.Format(L"c -- w:%d, h:%d", rect.right - rect.left, rect.bottom - rect.top);
	//::MessageBox(NULL, ts, L"title", MB_OK);
	CBrush brush;
	brush.CreateHatchBrush(6, RGB(0, 0x41, 0x66));
	dc.FillRect(rect, &brush);

	//g_PPU.dc = &dc;
	//g_PPU.showBG();
	
	// TODO: �ڴ˴������Ϣ����������
	
	// ��ҪΪ������Ϣ������ CWnd::OnPaint()
}



void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CString key;
	key.Format(L"����:%d", nChar);
	byte index = 0xff;
	switch (nChar)
	{
	case 67: //C��       A
		index = 0;
		break;
	case 88: //X��       B
		index = 1;
		break;
	case 16: //��shift�� ѡ��
		index = 2;
		break;
	case 13: //�س���     ��ʼ
		index = 3;
		break;
	case VK_UP:    //��
		index = 4;
		break;
	case VK_DOWN:  //��
		index = 5;
		break;
	case VK_LEFT:  //��
		index = 6;
		break;
	case VK_RIGHT: //��
		index = 7;
		break;
	default:
		break;
	}

	if (index < 0x08) {
		g_PPU.HAND_KEY[0][index] = g_PPU.HAND_KEY[1][index]  = 1;
	}
	//MessageBox(key);
}


void CChildView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CString key;
	key.Format(L"����:%d", nChar);
	byte index = 0xff;
	switch (nChar)
	{
	case 67: //C��       A
		index = 0;
		break;
	case 88: //X��       B
		index = 1;
		break;
	case 16: //��shift�� ѡ��
		index = 2;
		break;
	case 13: //�س���     ��ʼ
		index = 3;
		break;
	case VK_UP:    //��
		index = 4;
		break;
	case VK_DOWN:  //��
		index = 5;
		break;
	case VK_LEFT:  //��
		index = 6;
		break;
	case VK_RIGHT: //��
		index = 7;
		break;
	default:
		break;
	}

	if (index < 0x08) {
		g_PPU.HAND_KEY[0][index] = g_PPU.HAND_KEY[1][index] = 0;
	}
}
