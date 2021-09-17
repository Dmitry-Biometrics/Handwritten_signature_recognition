#include "stdafx.h"
#include "DlgBarEx.h"
#include "NCTestProj.h"
#include "bnc32.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgBarWithList   m_wndRightBar;
CDlgBarWithMenu   m_wndLeftBar;
CBottomDlgBar     m_wndBotDlgBar;

CRect startRBSize;			// исходные размеры левого бара
CRect startBBSize;			// исходные размеры нижнего бара



/////////////////////////////////////////////////////////////////////////////
// CDlgBarWithList

void CDlgBarWithList::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgBarWithList, CDialogBar)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL CDlgBarWithList::OnInitDialog()
{
	GetClientRect(&startRBSize);
	
	// Определяем положение списка на форме
	CRect myRect;
	m_ImageList.GetWindowRect(&myRect);
	ScreenToClient(myRect);
	startListSize.x = myRect.left;
	startListSize.y = myRect.top;
	startListSize.iWidth = myRect.right - myRect.left;
	startListSize.iHeight = myRect.bottom - myRect.top;

	// Определяем положение 1 кнопочки на форме ("Удалить образ")
	m_DelImageBtn.GetWindowRect(&myRect);
	ScreenToClient(myRect);
	startBut1Size.x = myRect.left;
	startBut1Size.y = myRect.top;
	startBut1Size.iWidth  = myRect.right - myRect.left;
	startBut1Size.iHeight = myRect.bottom - myRect.top;

	// Определяем положение 2 кнопочки на форме ("Очистить список")
	m_DelAllImagesBtn.GetWindowRect(&myRect);
	ScreenToClient(myRect);
	startBut2Size.x = myRect.left;
	startBut2Size.y = myRect.top;

	// Определяем положение 3 кнопочки на форме ("Добавить")
	m_AddImageBtn.GetWindowRect(&myRect);
	ScreenToClient(myRect);
	startBut3Size.x = myRect.left;
	startBut3Size.y = myRect.top;

	// Определяем положение 4 кнопочки на форме ("Обучить")
	m_LearnNetBtn.GetWindowRect(&myRect);
	ScreenToClient(myRect);
	startBut4Size.x = myRect.left;
	startBut4Size.y = myRect.top;

	if ( mode == NCTRAIN_MODE ) {
		// Режим обучения
		GetDlgItem(IDC_SECRET_RADIO)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_PUBLIC_RADIO)->ShowWindow(SW_HIDE);
		// Показываем нужные окошки/кнопочки
		m_ImageList.ShowWindow(SW_SHOW);
		m_DelImageBtn.ShowWindow(SW_SHOW);
		m_DelAllImagesBtn.ShowWindow(SW_SHOW);
		m_AddImageBtn.ShowWindow(SW_SHOW);
		m_LearnNetBtn.ShowWindow(SW_SHOW);

	} else {
		// Режим тестирования
		m_hWhiteBrush = CreateSolidBrush(RGB(255,255,255));
		// По умолчанию помечена кнопка "Открыто"
		CheckRadioButton(IDC_SECRET_RADIO, IDC_PUBLIC_RADIO, IDC_PUBLIC_RADIO);
		GetDlgItem(IDC_SECRET_RADIO)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_PUBLIC_RADIO)->ShowWindow(SW_SHOW);
		// Скрываем лишние окошки/кнопочки
		m_ImageList.ShowWindow(SW_HIDE);
		m_DelImageBtn.ShowWindow(SW_HIDE);
		m_DelAllImagesBtn.ShowWindow(SW_HIDE);
		m_AddImageBtn.ShowWindow(SW_HIDE);
		m_LearnNetBtn.ShowWindow(SW_HIDE);
	}

	bResizeRB = false;	// меняем размер окна, только после того как его нарисуем

	return TRUE;
}

BOOL CDlgBarWithList::CreateListBox(CListBox& listBox, UINT nIndex, UINT nID, 
								int nWidth)
{
	// Create the list box
	int xpos = 23;
	int ypos = 10;
	if (!listBox.Create(
			WS_BORDER|WS_VISIBLE|LBS_NOTIFY|LBS_STANDARD,
			CRect(xpos,ypos,xpos+nWidth,280), this, nID))
	{
		TRACE("Failed to create list-box\n");
		return FALSE;
	}

	return TRUE;
}

BOOL CDlgBarWithList::CreateButton(class CButton& button, UINT nIndex, UINT nID, CRect pos, LPCTSTR lpszButText)
{
	// Create the button
	if (!button.Create(lpszButText,
			WS_CHILD|WS_VISIBLE, 
			pos, this, nID))
	{
		TRACE("Failed to create button\n");
		return FALSE;
	}

	return TRUE;
}
BOOL CDlgBarWithList::OnCommand(int iNum, WPARAM wParam, LPARAM lParam) 
{
	if( wParam == IDOK && lParam == 0 )
	{

		CString strText;
		CString strCommand;
		if ( iNum == -1 )
		{
			// очистить список
			m_ImageList.ResetContent();
			
		}
		else
		{	
			// добавить образ в список
			strText.Format(_T("образ %d"), iNum);
			m_ImageList.AddString( strText );
			m_ImageList.SetCurSel(iNum-1);
		}
	}
	
	return CDialogBar::OnCommand(wParam, lParam);
}
// Функция перерисовки окна (используется для зажигания светофора)
BOOL CDlgBarWithList::OnEraseBkgnd(CDC* pDC)
{

	CDialogBar::OnEraseBkgnd(pDC);
	
	if ( mode == NCTEST_MODE )
	{
		CDC dcMem;
		CBitmap bmp1,bmp2;
		CRect rc;
		RECT xy;
	
		GetClientRect(rc);
		CPen bluePen;
		bluePen.CreatePen(PS_SOLID, 1, RGB(1, 0, 154));
		pDC->SelectObject(bluePen);
		rc.DeflateRect(1, 2);
		pDC->Rectangle(rc);
	
		dcMem.CreateCompatibleDC( pDC );
	
		// Рисуем светофор
		switch( iLight )
		{
			case 0: bmp1.LoadBitmap(IDB_SVET_OFF); break;
			case 1: bmp1.LoadBitmap(IDB_SVET_RED); break;
			case 2: bmp1.LoadBitmap(IDB_SVET_YEL); break;
			case 3: bmp1.LoadBitmap(IDB_SVET_GREEN); break;
		}
		dcMem.SelectObject(&bmp1);
		xy.left = 28;
		xy.top  = 25;
		pDC->BitBlt(xy.left, xy.top, rc.Width(), rc.Height(), &dcMem, 0, 0, SRCCOPY);

		// Рисуем эмблему
		//bmp2.LoadBitmap(IDB_EMBLEM);
		//dcMem.SelectObject(&bmp2);
		//xy.left = rc.right-100;
		//xy.top  = rc.bottom-160;
		//pDC->BitBlt(xy.left, xy.top, rc.Width(), rc.Height(), &dcMem, 0, 0, SRCCOPY);
	}
	bResizeRB = true;

	return TRUE;

}
// Задаём цвета
HBRUSH CDlgBarWithList::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if ( mode == NCTRAIN_MODE )
	{
		HBRUSH hbr = CDialogBar::OnCtlColor(pDC, pWnd, nCtlColor);
		return hbr;
	} else {
		if ( nCtlColor == CTLCOLOR_STATIC ) {		// меняем цвет статика
			pDC->SetBkColor(RGB(255,255,255));		// белый цвет
			return m_hWhiteBrush;
		}
		return CDialogBar::OnCtlColor(pDC, pWnd, nCtlColor);
	}

}
void CDlgBarWithList::OnSize(UINT nType, int cx, int cy)
{
	CDialogBar::OnSize(nType, cx, cy);

	if ( bResizeRB ) {
		// Меняем размер списка
		int iWidth,iHeight;
		iWidth  = startListSize.iWidth + cx - startRBSize.right;
		iHeight = startListSize.iHeight + cy - startRBSize.bottom;
		m_ImageList.MoveWindow(startListSize.x, startListSize.y, iWidth, iHeight);
		
		iWidth  = startBut1Size.iWidth + cx - startRBSize.right;
		iHeight = 25;
		m_DelImageBtn.MoveWindow(startBut1Size.x, startBut1Size.y + cy - startRBSize.bottom, iWidth, iHeight);
		m_DelAllImagesBtn.MoveWindow(startBut2Size.x, startBut2Size.y + cy - startRBSize.bottom, iWidth, iHeight);
		m_AddImageBtn.MoveWindow(startBut3Size.x, startBut3Size.y + cy - startRBSize.bottom, iWidth, iHeight);
		m_LearnNetBtn.MoveWindow(startBut4Size.x, startBut4Size.y + cy - startRBSize.bottom, iWidth, iHeight);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBottomDlgBar 
void CBottomDlgBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(CBottomDlgBar, CDialogBar)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
END_MESSAGE_MAP()
BOOL CBottomDlgBar::OnInitDialog()
{
	GetClientRect(&startBBSize);

	CRect myRect;

	// Определяем положение 5 кнопочки на форме ("Очистить")
	m_ClearBtn.GetWindowRect(&myRect);
	ScreenToClient(myRect);
	startBut5Size.x = myRect.left;
	startBut5Size.y = myRect.top;
	startBut5Size.iWidth  = myRect.right - myRect.left;
	startBut5Size.iHeight = myRect.bottom - myRect.top;

	// Определяем положение 6 кнопочки на форме ("Проверить")
	m_CheckBtn.GetWindowRect(&myRect);
	ScreenToClient(myRect);
	startBut6Size.x = myRect.left;
	startBut6Size.y = myRect.top;
	startBut6Size.iWidth  = myRect.right - myRect.left;
	startBut6Size.iHeight = myRect.bottom - myRect.top;

	bResizeBB = false;

	return TRUE;
}

BOOL CBottomDlgBar::CreateButton(class CButton& button, UINT nIndex, UINT nID, CRect pos, LPCTSTR lpszButText)
{
	// Create the button
	if (!button.Create(lpszButText,
		WS_CHILD|WS_VISIBLE, 
		pos, this, nID))
	{
		TRACE("Failed to create button\n");
		return FALSE;
	}

	return TRUE;
}

// Функция перерисовки
BOOL CBottomDlgBar::OnEraseBkgnd(CDC* pDC)
{
	CDialogBar::OnEraseBkgnd(pDC);

	CRect rc;
	GetClientRect(rc);

	CBrush blueBrush(RGB(1, 0, 154));
	CPen bluePen;
	bluePen.CreatePen(PS_SOLID, 1, RGB(1, 0, 154));

	// Рисуем синий треугольник
	pDC->SelectObject(&blueBrush);
	pDC->SelectObject(&bluePen);
	CPoint pts[3];
	pts[0].x = rc.left;
	pts[0].y = rc.bottom;
	pts[1].x = rc.left+132;
	pts[1].y = rc.top;
	pts[2].x = rc.left + 166;
	pts[2].y = rc.bottom;
	pDC->Polygon(pts, 3);
	// Рисуем синий прямоугольник
	pDC->Rectangle(rc.left, rc.top, pts[1].x, rc.bottom);

	bResizeBB = true;

	return TRUE;

}

void CBottomDlgBar::OnSize(UINT nType, int cx, int cy)
{
	CDialogBar::OnSize(nType, cx, cy);

	if ( bResizeBB ) 
	{
		// Меняем размер списка
		int x;
		x = startBut5Size.x + cx - startBBSize.right;
		m_ClearBtn.MoveWindow (	x, startBut5Size.y, startBut5Size.iWidth, startBut5Size.iHeight );
		x = startBut6Size.x + cx - startBBSize.right;
		m_CheckBtn.MoveWindow (	x, startBut6Size.y, startBut6Size.iWidth, startBut6Size.iHeight );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSmartListBox
CSmartListBox::CSmartListBox()
{
	m_lfHeight = -10;
	m_lfWeight = FW_NORMAL;
	m_strFaceName = _T("MS Sans Serif");
	m_nMaxStrings = NCIMAGE_COUNT;
}

BEGIN_MESSAGE_MAP(CSmartListBox, CListBox)
	ON_WM_CREATE()
END_MESSAGE_MAP()

int CSmartListBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CListBox::OnCreate(lpCreateStruct) == -1 )
		return -1;

	if( !CreateFont(m_lfHeight, m_lfWeight, m_strFaceName) )
	{
		TRACE0("Failed to create font for combo box\n");
		return -1;      // fail to create
	}

	return 0;
}

BOOL CSmartListBox::CreateFont(LONG lfHeight, LONG lfWeight, LPCTSTR lpszFaceName)
{
	//  Create a font for the combobox
	LOGFONT logFont;
	memset(&logFont, 0, sizeof(logFont));

	if (!::GetSystemMetrics(SM_DBCSENABLED))
	{
		// Since design guide says toolbars are fixed height so is the font.
		logFont.lfHeight = lfHeight;
		logFont.lfWeight = lfWeight;
		CString strDefaultFont = lpszFaceName;
		lstrcpy(logFont.lfFaceName, strDefaultFont);
		if (!m_font.CreateFontIndirect(&logFont))
		{
			TRACE("Could Not create font for combo\n");
			return FALSE;
		}
		SetFont(&m_font);
	}
	else
	{
		m_font.Attach(::GetStockObject(SYSTEM_FONT));
		SetFont(&m_font);
	}
	return TRUE;
}

int CSmartListBox::AddString(LPCTSTR str)
{

	if( _tcslen(str) == 0 )
		return -1;

	int oldIndex = FindStringExact(-1, str);
	if( oldIndex >= 0 )
		DeleteString(oldIndex);

	if( GetCount() == m_nMaxStrings )
		DeleteString(m_nMaxStrings-1);

	return CListBox::InsertString(0, str);
}

int CSmartListBox::InsertString(int index, LPCTSTR str)
{
	if( _tcslen(str) == 0 )
		return -1;

	int oldIndex = FindStringExact(-1, str);
	if( oldIndex >= 0 )
	{
		DeleteString(oldIndex);
		if( index >= oldIndex )
			--index;
	}

	if( GetCount() == m_nMaxStrings )
		DeleteString(m_nMaxStrings-1);

	return CListBox::InsertString(index, str);
}

/////////////////////////////////////////////////////////////////////////////
// CSmartButton
CSmartButton::CSmartButton()
{
	m_lfHeight = -10;
	m_lfWeight = FW_NORMAL;
	m_strFaceName = _T("MS Sans Serif");
}

BEGIN_MESSAGE_MAP(CSmartButton, CButton)
	ON_WM_CREATE()
END_MESSAGE_MAP()

int CSmartButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CButton::OnCreate(lpCreateStruct) == -1)
		return -1;

	if( !CreateFont(m_lfHeight, m_lfWeight, m_strFaceName) )
	{
		TRACE0("Failed to create font for combo box\n");
		return -1;      // fail to create
	}

	return 0;
}

BOOL CSmartButton::CreateFont(LONG lfHeight, LONG lfWeight, LPCTSTR lpszFaceName)
{
	//  Create a font for the button
	LOGFONT logFont;
	memset(&logFont, 0, sizeof(logFont));

	if (!::GetSystemMetrics(SM_DBCSENABLED))
	{
		// Since design guide says toolbars are fixed height so is the font.
		logFont.lfHeight = lfHeight;
		logFont.lfWeight = lfWeight;
		CString strDefaultFont = lpszFaceName;
		lstrcpy(logFont.lfFaceName, strDefaultFont);
		if (!m_font.CreateFontIndirect(&logFont))
		{
			TRACE("Could Not create font for combo\n");
			return FALSE;
		}
		SetFont(&m_font);
	}
	else
	{
		m_font.Attach(::GetStockObject(SYSTEM_FONT));
		SetFont(&m_font);
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CMainMenuDlgBar
void CDlgBarWithMenu::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgBarWithMenu, CDialogBar)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// Функция перерисовки окна 
BOOL CDlgBarWithMenu::OnEraseBkgnd(CDC* pDC)
{
	CDialogBar::OnEraseBkgnd(pDC);

	CDC dcMem;
	CBitmap bmp;
	CRect rc;
	RECT xy;

	GetClientRect(rc);
	
	CBrush blueBrush(RGB(1, 0, 154));
	CPen bluePen;
	bluePen.CreatePen(PS_SOLID, 1, RGB(1, 0, 154));
	// Рисуем синий треугольник
	pDC->SelectObject(blueBrush);
	pDC->SelectObject(bluePen);
	CPoint pts[3];
	pts[0].x = rc.left;
	pts[0].y = rc.bottom-290;
	pts[1].x = rc.left;
	pts[1].y = rc.bottom;
	pts[2].x = rc.right;
	pts[2].y = rc.bottom - 170;
	pDC->Polygon(pts, 3);

	// Рисуем синий прямоугольник
	xy.left = rc.left;
	xy.top  = rc.bottom-170;
	xy.right  = rc.right;
	xy.bottom = rc.bottom;
	pDC->Rectangle(&xy);
	
  // Рисуем эмблему
  dcMem.CreateCompatibleDC( pDC );
  bmp.LoadBitmap(IDB_GERBBLUE);
  dcMem.SelectObject(&bmp);
  xy.left = 1;
  xy.top  = rc.bottom - 160;
  pDC->BitBlt(xy.left, xy.top, rc.Width(), rc.Height(), &dcMem, 0, 0, SRCCOPY);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
