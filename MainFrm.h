
// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "ChildView.h"
#include <mmsystem.h>
#include <dsound.h>

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame(NES**);
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// 特性
public:
	NES** nes;
// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	static UINT Sound(LPVOID param);
	static UINT Game(LPVOID param);

protected:  // 控件条嵌入成员
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;
	CChildView    m_wndView;
	ID2D1Bitmap*  m_pBitmap;

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	DECLARE_MESSAGE_MAP()

};


