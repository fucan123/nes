
// ChildView.cpp : CChildView 类的实现
//

#include "stdafx.h"
#include "nes_mfc.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//CPU全局变量
extern CPU g_CPU;
//CPU全局变量
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



// CChildView 消息处理程序

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
	CPaintDC dc(this); // 用于绘制的设备上下文
	GetClientRect(&rect);
	CString ts;
	ts.Format(L"c -- w:%d, h:%d", rect.right - rect.left, rect.bottom - rect.top);
	//::MessageBox(NULL, ts, L"title", MB_OK);
	CBrush brush;
	brush.CreateHatchBrush(6, RGB(0, 0x41, 0x66));
	dc.FillRect(rect, &brush);

	//g_PPU.dc = &dc;
	//g_PPU.showBG();
	
	// TODO: 在此处添加消息处理程序代码
	
	// 不要为绘制消息而调用 CWnd::OnPaint()
}



void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CString key;
	key.Format(L"按下:%d", nChar);
	byte index = 0xff;
	switch (nChar)
	{
	case 67: //C键       A
		index = 0;
		break;
	case 88: //X键       B
		index = 1;
		break;
	case 16: //右shift键 选择
		index = 2;
		break;
	case 13: //回车键     开始
		index = 3;
		break;
	case VK_UP:    //上
		index = 4;
		break;
	case VK_DOWN:  //下
		index = 5;
		break;
	case VK_LEFT:  //左
		index = 6;
		break;
	case VK_RIGHT: //右
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
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CString key;
	key.Format(L"弹起:%d", nChar);
	byte index = 0xff;
	switch (nChar)
	{
	case 67: //C键       A
		index = 0;
		break;
	case 88: //X键       B
		index = 1;
		break;
	case 16: //右shift键 选择
		index = 2;
		break;
	case 13: //回车键     开始
		index = 3;
		break;
	case VK_UP:    //上
		index = 4;
		break;
	case VK_DOWN:  //下
		index = 5;
		break;
	case VK_LEFT:  //左
		index = 6;
		break;
	case VK_RIGHT: //右
		index = 7;
		break;
	default:
		break;
	}

	if (index < 0x08) {
		g_PPU.HAND_KEY[0][index] = g_PPU.HAND_KEY[1][index] = 0;
	}
}
