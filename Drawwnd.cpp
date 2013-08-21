// drawwnd.cpp : implementation file
//

#include "stdafx.h"
#include "Saver.h"
#include "drawwnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

LPCTSTR CDrawWnd::m_lpszClassName = NULL;

/////////////////////////////////////////////////////////////////////////////
// CDrawWnd

CDrawWnd::CDrawWnd(BOOL bAutoDelete)
{
	init=false;
	m_bAutoDelete = bAutoDelete;
}

CDrawWnd::~CDrawWnd()
{
}


BEGIN_MESSAGE_MAP(CDrawWnd, CWnd)
	//{{AFX_MSG_MAP(CDrawWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawWnd message handlers

void CDrawWnd::OnPaint() 
{
	if(!init)
	{
		CPaintDC dc(this); // device context for painting
		CBrush brush(RGB(0,0,0));
		CRect rect;
		GetClientRect(rect);
		dc.FillRect(&rect, &brush);
		init=true;
	}

	probe.run();
	Sleep(4);
	Invalidate(FALSE);

	// Do not call CWnd::OnPaint() for painting messages
}

int CDrawWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

BOOL CDrawWnd::Create(DWORD dwExStyle, DWORD dwStyle, const RECT& rect, 
	CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
    // Register a class with no cursor
	if (m_lpszClassName == NULL)
	{
    	m_lpszClassName = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW,
			::LoadCursor(AfxGetResourceHandle(), 
			MAKEINTRESOURCE(IDC_NULLCURSOR)));
	}

	// TODO: Add your specialized code here and/or call the base class
	return CreateEx(dwExStyle, m_lpszClassName, _T(""), dwStyle, 
		rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, 
		pParentWnd->GetSafeHwnd(), NULL, NULL );
}

void CDrawWnd::PostNcDestroy() 

{
	if (m_bAutoDelete)
		delete this;
}

/****************************************************************************/
