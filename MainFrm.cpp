
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "nes_mfc.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

//CPU全局变量
extern CPU g_CPU;
//CPU全局变量
extern PPU g_PPU;

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 创建一个视图以占用框架的工作区
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("未能创建视图窗口\n");
		return -1;
	}

	/*if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}*/

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 如果不需要可停靠工具栏，则删除这三行
	/*m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);*/
	AfxBeginThread(Game, this);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	cs.cx = 800;
	cs.cy = 600;
	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// 将焦点前移到视图窗口
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// 让视图第一次尝试该命令
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// 否则，执行默认处理
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

UINT CMainFrame::Game(LPVOID param) {
	Sleep(100);
	CString ts;
	ts.Format(L"PC:%x", g_CPU.R.PC);
	//::MessageBox(NULL, ts, L"t", MB_OK);
	CMainFrame* pFrame = (CMainFrame*)param;
	CDC* dc = pFrame->m_wndView.GetDC();
	CDC dcImage;
	if (!dcImage.CreateCompatibleDC(dc)) {
		::MessageBox(NULL, L"create failed", L"title", MB_OK);
		return 0;
	}
	if (!dc) {
		::MessageBox(NULL, L"!dc", L"title", MB_OK);
	}
	//::MessageBox(NULL, L"afx", L"title", MB_OK);
	//::MessageBox(NULL, L"game", L"title", MB_OK);

	/*CRect rect;
	//GetClientRect(&rect);
	CBrush brush;
	brush.CreateHatchBrush(6, RGB(0, 0x41, 0x66));
	//dc.FillRect(rect, &brush);

	//MessageBox(L"start");
	byte cb[256 * 240 * 4];
	for (int i = 0; i < 256 * 240; i++) {
		int index = i * 4;
		cb[index + 0] = i & 0x0f;
		cb[index + 1] = i & 0xf0;
		cb[index + 2] = i & 0xff;
		cb[index + 3] = 0;
	}

	CString ms;
	CBitmap bm;
	bm.CreateBitmap(256, 240, 1, 32, cb);
	BITMAP  bmp;
	bm.GetBitmap(&bmp);

	ms.Format(L"w=%d,h=%d,p=%d,bc:%d,bits:%d,error:%d",
		bmp.bmWidth, bmp.bmHeight, bmp.bmPlanes, bmp.bmBitsPixel, bmp.bmBits, GetLastError());
	//MessageBox(ms);

	CBitmap* pOldBitmap = dcImage.SelectObject(&bm);

	dc->StretchBlt(0, 0, 256, 240, &dcImage, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
	//dc.BitBlt(10, 10, bmp.bmWidth, bmp.bmHeight, &dcImage, 0, 0, SRCCOPY);
	// TODO: 在此处添加消息处理程序代码
	dcImage.SelectObject(pOldBitmap);
	return 0;*/
	g_CPU.reset();
	//一条扫描线时间
	double line_time = 1 / 50 / 312; //每秒50帧 一帧312条扫描线
	int line = 0; //第几条扫描线
	LARGE_INTEGER freq, stime, ctime;
	QueryPerformanceFrequency(&freq); //获取时钟频率
	CString tt;
	tt.Format(L"%ld", freq.QuadPart);
	//::MessageBox(NULL, tt, L"title", MB_OK);
	QueryPerformanceCounter(&stime); //113.6825
	byte images[256 * 240 * 4];
	memset(images, 0, sizeof(images));
	CString xs;
	xs.Format(L"thread p:%d", g_CPU.pause);
	//::MessageBox(NULL, xs, L"t", MB_OK);
	g_CPU.opnum = 0;
	g_CPU.exec_opnum = 0x7fffffff;
	while (true) {
		while (g_CPU.opnum < g_CPU.exec_opnum) {
			//xs.Format(L"thread %d p:%d", g_CPU.opnum, g_CPU.pause);
			//::MessageBox(NULL, xs, L"t", MB_OK);
			if (g_CPU.pause && !g_CPU.step) {
				CString tt;
				tt.Format(L"pasue:%d, all num:%d, opnum:%d", g_CPU.pause, g_CPU.exec_opnum, g_CPU.opnum);
				::MessageBox(NULL, tt, L"title", MB_OK);
				continue;
			}
			//::MessageBox(NULL, tt, L"title", MB_OK);
			QueryPerformanceCounter(&ctime); //当前时间
			double dim = (double)(ctime.QuadPart - stime.QuadPart) / (double)freq.QuadPart;
			if (dim >= line_time) {
				int exec_cycles = 114;
				if (line == 240) {
					if (g_PPU.IS_NMI) {
						CString tt;
						tt.Format(L"opcode:%X", g_CPU.R.PC);
						//::MessageBox(NULL, tt , L"title", MB_OK);
						exec_cycles -= 7;
						g_CPU.NMI();
						exec_cycles -= 7;
					}
				}
				if (0 && g_CPU.step) {
					g_CPU.exec(1);
					CString tt;
					tt.Format(L"step:%d, all num:%d, opnum:%d", g_CPU.step, g_CPU.exec_opnum, g_CPU.opnum);
					//::MessageBox(NULL, tt, L"title", MB_OK);
					//g_CPU.step = false;
					//g_CPU.pause = true;
				}
				else {
					int num = g_CPU.exec(exec_cycles);
				}

				//执行cpu指令 113.6825周期
				if (line < 240) {
					g_PPU.scanfLine(line, images);
					CBitmap bm;
					bm.CreateBitmap(256, 240, 1, 32, images);
					BITMAP  bmp;
					bm.GetBitmap(&bmp);
					CBitmap* pOldBitmap = dcImage.SelectObject(&bm);

					int width = pFrame->m_wndView.rect.right - pFrame->m_wndView.rect.left;
					int height = pFrame->m_wndView.rect.bottom - pFrame->m_wndView.rect.top;
					dc->StretchBlt(0, 0, width, height, &dcImage, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
					//dc.BitBlt(10, 10, bmp.bmWidth, bmp.bmHeight, &dcImage, 0, 0, SRCCOPY);
					// TODO: 在此处添加消息处理程序代码
					dcImage.SelectObject(pOldBitmap);
					//绘制扫描线
				}
				if (++line == 312) //全部312扫描完成
					line = 0;
			}
		}
		if (g_CPU.opnum > 0 && g_CPU.exec_opnum == g_CPU.opnum) {
			//::MessageBox(NULL, L"set 0", L"title", MB_OK);
			g_CPU.exec_opnum = 0;
			g_CPU.opnum = 0;
		}
	}
	return 0;
}

