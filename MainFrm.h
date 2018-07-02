
// MainFrm.h : CMainFrame ��Ľӿ�
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

// ����
public:
	NES** nes;
// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	static UINT Sound(LPVOID param);
	static UINT Game(LPVOID param);

protected:  // �ؼ���Ƕ���Ա
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;
	CChildView    m_wndView;
	ID2D1Bitmap*  m_pBitmap;

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	DECLARE_MESSAGE_MAP()

};


