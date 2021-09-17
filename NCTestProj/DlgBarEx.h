//////////////////////////////////////////////////////////////////////////
// Структура - хранилище начальных координат и размеров контрола
struct  ControlCoord		// исходные размеры 
{
	int x;					// координаты левого
	int y;					// верхнего угла 
	int iWidth;				// ширина
	int iHeight;			// высота
};

/////////////////////////////////////////////////////////////////////////////
// CSmartListBox
class CSmartListBox : public CListBox
{
protected:
	CFont			m_font;
public:
					CSmartListBox();

	int				m_nMaxStrings;
	LONG			m_lfHeight;
	LONG			m_lfWeight;
	CString			m_strFaceName;

	BOOL			CreateFont(LONG lfHeight, LONG lfWeight, LPCTSTR lpszFaceName);
	int				AddString (LPCTSTR str);
	int				InsertString(int index, LPCTSTR str);

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
};

// CSmartButton
class CSmartButton : public CButton
{
protected:
	CFont			m_font;
public:
					CSmartButton();

	LONG			m_lfHeight;
	LONG			m_lfWeight;
	CString			m_strFaceName;

	BOOL			CreateFont(LONG lfHeight, LONG lfWeight, LPCTSTR lpszFaceName);

	// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
};

#define USE_SMART_LIST_BOX
#define USE_SMART_BUTTON

class CDlgBarWithList : public CDialogBar
{
public:

#ifdef USE_SMART_LIST_BOX
	CSmartListBox	m_ImageList;
#else
	CListBox		m_ImageList;
#endif

#ifdef USE_SMART_BUTTON
	CSmartButton	m_DelImageBtn;
	CSmartButton	m_DelAllImagesBtn;
	CSmartButton	m_AddImageBtn;
	CSmartButton	m_LearnNetBtn;
#else
	CButton			m_DelImageBtn;
	CButton			m_DelAllImagesBtn;
	CButton			m_AddImageBtn;
	CButton			m_LearnNetBtn;
#endif

// Overrides
	// ClassWizard generated virtual function overrides
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(int iNum, WPARAM wParam, LPARAM lParam);
	BOOL		 CreateListBox(class CListBox& listBox, UINT nIndex, UINT nID, int nWidth);
	BOOL		 CreateButton(class CButton& button, UINT nIndex, UINT nID, CRect pos, LPCTSTR lpszButText);
	
// Generated message map functions
protected:
	//{{AFX_MSG(CToolBarWithCombo)
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	
	int iLight;					// выбранный цвет сигнала светофора
	int mode;					// выбранный режим
	HBRUSH  m_hWhiteBrush;
	bool bResizeRB;				// изменение размеров левого бара

	ControlCoord startListSize;
	ControlCoord startBut1Size;
	ControlCoord startBut2Size;
	ControlCoord startBut3Size;
	ControlCoord startBut4Size;
};

extern CDlgBarWithList   m_wndRightBar;

class CBottomDlgBar : public CDialogBar
{
public:
#ifdef USE_SMART_BUTTON
	CSmartButton	m_ClearBtn;
	CSmartButton	m_CheckBtn;
#else
	CButton			m_ClearBtn;
	CButton			m_CheckBtn;
#endif

	// Overrides
	// ClassWizard generated virtual function overrides
	virtual BOOL OnInitDialog();
	BOOL		 CreateButton(class CButton& button, UINT nIndex, UINT nID, CRect pos, LPCTSTR lpszButText);

	// Generated message map functions
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()

public:
	bool bResizeBB;				// изменение размеров нижнего бара
	ControlCoord startBut5Size;
	ControlCoord startBut6Size;

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
extern CBottomDlgBar m_wndBotDlgBar;

class CDlgBarWithMenu : public CDialogBar
{
public:
	// Generated message map functions
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
extern CDlgBarWithMenu   m_wndLeftBar;

/////////////////////////////////////////////////////////////////////////////


