
// MainFrm.cpp : CMainFrame ���ʵ��
//

#include "stdafx.h"
#include "nes_mfc.h"

#include "MainFrm.h"
#include "NES/NES.h"
#include "NES/CPU.h"

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
	ID_SEPARATOR,           // ״̬��ָʾ��
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

//CPUȫ�ֱ���
extern CPU g_CPU;
//CPUȫ�ֱ���
extern PPU g_PPU;

// CMainFrame ����/����

CMainFrame::CMainFrame(NES** p)
{
	// TODO: �ڴ���ӳ�Ա��ʼ������
	nes = p;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// ����һ����ͼ��ռ�ÿ�ܵĹ�����
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("δ�ܴ�����ͼ����\n");
		return -1;
	}
	m_wndView.nes = *nes;
	/*if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("δ�ܴ���������\n");
		return -1;      // δ�ܴ���
	}*/

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("δ�ܴ���״̬��\n");
		return -1;      // δ�ܴ���
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	m_wndStatusBar.SetPaneText(2, L"mm");
	// TODO: �������Ҫ��ͣ������������ɾ��������
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
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	cs.cx = 800;
	cs.cy = 600;
	return TRUE;
}

// CMainFrame ���

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


// CMainFrame ��Ϣ�������

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// ������ǰ�Ƶ���ͼ����
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// ����ͼ��һ�γ��Ը�����
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// ����ִ��Ĭ�ϴ���
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

UINT CMainFrame::Game(LPVOID param) {
	SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);
	Sleep(100);
	CString ts;
	//ts.Format(L"PC:%x", nes->cpu->R.PC);
	//::MessageBox(NULL, ts, L"t", MB_OK);
	CMainFrame* pFrame = (CMainFrame*)param;
	::SendMessageA(pFrame->m_wndStatusBar.m_hWnd, SB_SETTEXT, 2, (LPARAM)LPCTSTR(ts));

	NES* nes = *(pFrame->nes);
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
	ID2D1Factory* m_pDirect2dFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;
	D2D1_RECT_U imgrect;
	RECT rc;
	HWND hwnd = pFrame->m_wndView.m_hWnd;
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);

	::GetClientRect(hwnd, &rc);
	int scale = (GetDeviceCaps(::GetDC(hwnd), LOGPIXELSX)) / (float)100 / 0.96;//������ű���  
	ts.Format(L"w:%d, h:%d, s:%d", rc.right-rc.left, rc.bottom-rc.top, scale);
	//::MessageBox(NULL, ts, L"title", MB_OK);
	D2D1_SIZE_U size = D2D1::SizeU
	(
		rc.right - rc.left,
		rc.bottom - rc.top
	);
	// Create a Direct2D render target.  
	m_pDirect2dFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(hwnd, size, D2D1_PRESENT_OPTIONS_IMMEDIATELY),//�������������ò��ȴ���ֱͬ����Ĭ�ϴ�ֱͬ��ʱ���ˢ��Ƶ��Ϊ�Կ�ˢ��Ƶ�ʣ�һ��60FPS  
		&m_pRenderTarget
	);
	//����λͼ  
	D2D1_SIZE_U imgsize = D2D1::SizeU(256, 240);
	D2D1_PIXEL_FORMAT pixelFormat =  //λͼ���ظ�ʽ����  
	{
		DXGI_FORMAT_B8G8R8A8_UNORM, //�ò�������ͼ�������������ظ�ʽ����ΪRGBA���ɸ�����Ҫ��Ϊ��ĸ�ʽ��ֻ�Ǻ�������ݿ���Ҫ����Ӧ�ĵ���  
		D2D1_ALPHA_MODE_IGNORE
	};
	D2D1_BITMAP_PROPERTIES prop =  //λͼ������Ϣ����  
	{
		pixelFormat,
		imgsize.width,
		imgsize.height
	};
	m_pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);//����ͼ��Ϊ�����ģʽ 
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
	// TODO: �ڴ˴������Ϣ����������
	dcImage.SelectObject(pOldBitmap);
	return 0;*/
	//nes->cpu->reset();
	//һ��ɨ����ʱ��
	double line_time = 1.0 / 50.0 / 312.0; //ÿ��50֡ һ֡312��ɨ����
	int line = 0; //�ڼ���ɨ����
	LARGE_INTEGER freq, stime, ctime;
	QueryPerformanceFrequency(&freq); //��ȡʱ��Ƶ��
	CString tt;
	tt.Format(L"%ld, line time:%.20f", freq.QuadPart, line_time);
	//::MessageBox(NULL, tt, L"title", MB_OK);
	QueryPerformanceCounter(&stime); //113.6825
	tt.Format(L"%ld %ld", freq.QuadPart, stime.QuadPart);
	//::MessageBox(NULL, tt, L"title", MB_OK);
	byte images[256 * 240 * 4];
	memset(images, 0, sizeof(images));
	imgrect.left = 0;
	imgrect.right = 256;
	imgrect.top = 0;
	imgrect.bottom = 240;
	

	nes->cpu->images = images;
	CString xs;
	xs.Format(L"thread p:%d", nes->cpu->pause);
	//pFrame->m_wndStatusBar.SetPaneText(2, L"xxx");
	//::MessageBox(NULL, xs, L"t", MB_OK);
	nes->cpu->opnum = 0;
	nes->cpu->exec_opnum = 0;
	//nes->cpu->exec_opnum =  0x7fffffff;
	while (true) {
		while (nes->cpu->opnum < nes->cpu->exec_opnum) {
			//xs.Format(L"thread %d p:%d", nes->cpu->opnum, nes->cpu->pause);
			//::MessageBox(NULL, xs, L"t", MB_OK);
			if (nes->cpu->pause && !nes->cpu->step) {
				CString tt;
				tt.Format(L"pasue:%d, all num:%d, opnum:%d", nes->cpu->pause, nes->cpu->exec_opnum, nes->cpu->opnum);
				::MessageBox(NULL, tt, L"title", MB_OK);
				continue;
			}
			//::MessageBox(NULL, tt, L"title", MB_OK);
			QueryPerformanceCounter(&ctime); //��ǰʱ��
			double dim = (double)(ctime.QuadPart - stime.QuadPart) / (double)freq.QuadPart;
			/*CString tt;
			tt.Format(L"%.6fMS, %lu=%lu", dim * 1000, ctime.QuadPart, stime.QuadPart);
			::MessageBox(NULL, tt, L"title", MB_OK);*/
			if (dim >= line_time) {
				stime = ctime;
				int exec_cycles = 114;
				//xs.Format(L"%d", line);
				//::SendMessageA(pFrame->m_wndStatusBar.m_hWnd, SB_SETTEXT, 2, (LPARAM)LPCTSTR(xs));
				if (line == 240) {
					if (nes->ppu->IS_NMI) {
						//CString tt;
						//tt.Format(L"opcode:%X", nes->cpu->R.PC);
						//::MessageBox(NULL, tt , L"title", MB_OK);
						exec_cycles -= 7;
						nes->cpu->NMI();
						exec_cycles -= 7;
					}
					HRESULT r;
					ID2D1Bitmap* m_pBitmap;
					if (r = m_pRenderTarget->CreateBitmap(imgsize, images, imgsize.width * 4, &prop, &m_pBitmap)) {
						CString f;
						f.Format(L"����ʧ��:%X,%d", r, GetLastError());
						::MessageBox(NULL, f, L"title", MB_OK);
					}
					m_pRenderTarget->BeginDraw();//����ʾˢ��Ƶ���й�ϵ  
					if (m_pBitmap) {
						m_pBitmap->CopyFromMemory(&imgrect, images, 256 * 4);

						float w = rc.right - rc.left;
						float h = rc.bottom - rc.top;

						m_pRenderTarget->DrawBitmap(m_pBitmap, D2D1::RectF(0, 0, 620, 400));//�þ��δ�С���ܵ�"�����ı���Ӧ�ú�������Ŀ�Ĵ�С:xxx%"��Ӱ��  
						m_pRenderTarget->EndDraw();
						m_pBitmap->Release();
					}
					
				}
				if (0 && nes->cpu->step) {
					nes->cpu->exec(1);
					CString tt;
					tt.Format(L"step:%d, all num:%d, opnum:%d", nes->cpu->step, nes->cpu->exec_opnum, nes->cpu->opnum);
					//::MessageBox(NULL, tt, L"title", MB_OK);
					//nes->cpu->step = false;
					//nes->cpu->pause = true;
				}
				else {
					int num = nes->cpu->exec(exec_cycles);
				}

				//ִ��cpuָ�� 113.6825����
				if (line < 240) {
					nes->ppu->scanfLine(line, images);
					/*CBitmap bm;
					bm.CreateBitmap(256, 240, 1, 32, images);
					BITMAP  bmp;
					bm.GetBitmap(&bmp);
					CBitmap* pOldBitmap = dcImage.SelectObject(&bm);

					int width = pFrame->m_wndView.rect.right - pFrame->m_wndView.rect.left;
					int height = pFrame->m_wndView.rect.bottom - pFrame->m_wndView.rect.top;
					dc->StretchBlt(0, 0, width, height, &dcImage, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
					//dc.BitBlt(10, 10, bmp.bmWidth, bmp.bmHeight, &dcImage, 0, 0, SRCCOPY);
					// TODO: �ڴ˴������Ϣ����������
					dcImage.SelectObject(pOldBitmap);*/
					//����ɨ����
					/*CBitmap bm;
					bm.CreateBitmap(256, 240, 1, 32, images);
					BITMAP  bmp;
					bm.GetBitmap(&bmp);
					CBitmap* pOldBitmap = dcImage.SelectObject(&bm);

					int width = pFrame->m_wndView.rect.right - pFrame->m_wndView.rect.left;
					int height = pFrame->m_wndView.rect.bottom - pFrame->m_wndView.rect.top;
					dc->StretchBlt(0, 0, width, height, &dcImage, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
					//dc.BitBlt(10, 10, bmp.bmWidth, bmp.bmHeight, &dcImage, 0, 0, SRCCOPY);
					// TODO: �ڴ˴������Ϣ����������
					dcImage.SelectObject(pOldBitmap);*/
					/*HRESULT r;
					ID2D1Bitmap* m_pBitmap;
					if (r = m_pRenderTarget->CreateBitmap(imgsize, images, imgsize.width * 4, &prop, &m_pBitmap)) {
						CString f;
						f.Format(L"����ʧ��:%X,%d", r, GetLastError());
						::MessageBox(NULL, f, L"title", MB_OK);
					}
					m_pRenderTarget->BeginDraw();//����ʾˢ��Ƶ���й�ϵ  
					if (m_pBitmap) {
						m_pBitmap->CopyFromMemory(&imgrect, images, 256 * 4);

						float w = rc.right - rc.left;
						float h = rc.bottom - rc.top;

						m_pRenderTarget->DrawBitmap(m_pBitmap, D2D1::RectF(0, 0, 620, 400));//�þ��δ�С���ܵ�"�����ı���Ӧ�ú�������Ŀ�Ĵ�С:xxx%"��Ӱ��  
						m_pRenderTarget->EndDraw();
						m_pBitmap->Release();
					}*/
					
				}
				if (++line == 312) //ȫ��312ɨ�����
					line = 0;
			}
		}
		if (nes->cpu->opnum > 0 && nes->cpu->exec_opnum == nes->cpu->opnum) {
			//::MessageBox(NULL, L"set 0", L"title", MB_OK);
			nes->cpu->exec_opnum = 0;
			nes->cpu->opnum = 0;
		}
	}
	return 0;
}

