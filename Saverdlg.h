// Saverdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSaverDlg dialog

class CSaverDlg : public CDialog
{
// Construction
public:
	CSaverDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSaverDlg)
	enum { IDD = IDD_SAVER_DIALOG };
	CEdit	m_fileName;
	CString	m_nName;
	BOOL	m_musicDisable;
	//}}AFX_DATA
	COLORREF m_color;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaverDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
//	CDrawWnd m_wndPreview; // don't autodelete

	// Generated message map functions
	//{{AFX_MSG(CSaverDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
