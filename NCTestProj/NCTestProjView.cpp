// NCTestProjView.cpp : implementation of the CNCTestProjView class
//

#include "stdafx.h"
#include "NCTestProj.h"

#include "NCTestProjDoc.h"
#include "NCTestProjView.h"
#include "../Tools/FileManipulations.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNCTestProjView

IMPLEMENT_DYNCREATE(CNCTestProjView, CFormView)

BEGIN_MESSAGE_MAP(CNCTestProjView, CFormView)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CNCTestProjView construction/destruction

CNCTestProjView::CNCTestProjView()
	: CFormView(CNCTestProjView::IDD)
{
	// TODO: add construction code here

}

CNCTestProjView::~CNCTestProjView()
{
}

void CNCTestProjView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL CNCTestProjView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CNCTestProjView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// Загружаем картинку с избой
	HBITMAP hBmp = NULL;
	objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\pnzgu.bmp");
	if ( hBmp = (HBITMAP) LoadImage(0, objFileManip.tofileFullPath, IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_DEFAULTSIZE))
	{
		m_cBmp.Attach(hBmp);
		BITMAP Bmp;
		CClientDC dc(this); // device context for painting
		dcMem.CreateCompatibleDC( &dc );
		dcMem.SelectObject(m_cBmp);
		m_cBmp.GetBitmap(&Bmp);
		m_iBmpHeight = Bmp.bmHeight;		// высота картинки
		m_iBmpWidth  = Bmp.bmWidth;			// ширина картинки
	}
	else
		MessageBox(_T("Не найден файл ..Data\\pniei.bmp"));

//	GetParentFrame()->RecalcLayout();
//	ResizeParentToFit();
}


// CNCTestProjView diagnostics

#ifdef _DEBUG
void CNCTestProjView::AssertValid() const
{
	CFormView::AssertValid();
}

void CNCTestProjView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CNCTestProjDoc* CNCTestProjView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNCTestProjDoc)));
	return (CNCTestProjDoc*)m_pDocument;
}
#endif //_DEBUG

// CNCTestProjView message handlers

BOOL CNCTestProjView::OnEraseBkgnd(CDC* pDC)
{
	CFormView::OnEraseBkgnd(pDC);

	CRect rc;
	GetClientRect(rc);

	CBrush blueBrush(RGB(1, 0, 154));
	CBrush whiteBrush(RGB(255, 255, 255));
	CPen bluePen;
	bluePen.CreatePen(PS_SOLID, 1, RGB(1, 0, 154));

	// Рисуем 2 синих треугольника
	pDC->SelectObject(blueBrush);
	pDC->SelectObject(bluePen);
	CPoint pts[3];
	pts[0].x = rc.left;
	pts[0].y = rc.bottom-166;
	pts[1].x = rc.left;
	pts[1].y = rc.bottom;
	pts[2].x = rc.left + 166;
	pts[2].y = rc.bottom;
	pDC->Polygon(pts, 3);

// было
	pts[0].x = rc.right - 288;
	pts[0].y = rc.top;
	pts[1].x = rc.right;
	pts[1].y = rc.top;
	pts[2].x = rc.right;
	pts[2].y = rc.top + 288;
	pDC->Polygon(pts, 3);
// стало
	//pts[0].x = rc.right*80/668;
	//pts[0].y = rc.top;
	//pts[1].x = rc.right;
	//pts[1].y = rc.top;
	//pts[2].x = rc.right;
	//pts[2].y = rc.bottom*293/450;
	//pDC->Polygon(pts, 3);

	// Рисуем белый прямоугольник
	RECT xy;
	pDC->SelectObject(whiteBrush);
	xy.left = rc.right*182/668;
	xy.top  = rc.top+20;
	xy.right  = rc.right*644/668;
	xy.bottom = rc.bottom*300/450;
	pDC->Rectangle(&xy);

	// Рисуем избу и растягиваем если надо
	xy.left += 5;						// координата Хо
	xy.top  += 5;						// координата Yo
	xy.right = xy.right - xy.left - 5;	// новая ширина картинки
	xy.bottom = xy.bottom - xy.top - 5;	// новая высота картинки
	pDC->StretchBlt(xy.left,xy.top,xy.right,xy.bottom,&dcMem,0,0,m_iBmpWidth,m_iBmpHeight,SRCCOPY);

	//Добавляем текст
	CFont font;
	LOGFONT lf;
	TCHAR strBuffer[100];
	memset(&lf, 0, sizeof(LOGFONT));       
	lf.lfHeight = 18;      
	lf.lfWeight = 700;
	_tcscpy_s(lf.lfFaceName,  _T("MS Sans Serif"));
	VERIFY(font.CreateFontIndirect(&lf));  
	// Do something with the font just created...
	CClientDC dc(this);
	CFont* def_font = dc.SelectObject(&font);
	//Выбор фона шрифта
	dc.SetBkMode(TRANSPARENT);

	//Вывод самого текста
	_tcscpy_s(strBuffer, _T("BioNeuroAutograph 2.0 - modeling environment"));
	xy.left = rc.right - 360;
	xy.top  = rc.bottom*320/473;
	dc.TextOut(xy.left, xy.top, strBuffer, (int)_tcslen(strBuffer));

	_tcscpy_s(strBuffer, _T("artificial neural networks. Non-commercial product development"));
	xy.left = rc.right - 540;
	xy.top  = rc.bottom*345/473;
	dc.TextOut(xy.left, xy.top, strBuffer, (int)_tcslen(strBuffer));

	_tcscpy_s(strBuffer, _T("Penza State University"));
	xy.left = rc.right - 351;
	xy.top  = rc.bottom*370/473;
	dc.TextOut(xy.left, xy.top, strBuffer, (int)_tcslen(strBuffer));

	_tcscpy_s(strBuffer, _T("Benefits of replacing classic artificial neurons"));
	xy.left = rc.right - 465;
	xy.top  = rc.bottom*390/473;
	dc.TextOut(xy.left, xy.top, strBuffer, (int)_tcslen(strBuffer));

	_tcscpy_s(strBuffer, _T("per neurons geometric mean and harmonic mean"));
	xy.left = rc.right - 507;
	xy.top  = rc.bottom*410/473;
	dc.TextOut(xy.left, xy.top, strBuffer, (int)_tcslen(strBuffer));

	_tcscpy_s(strBuffer, _T("PSU Penza 2020."));
	xy.left = rc.right - 158;
	xy.top  = rc.bottom*445/473;
	dc.TextOut(xy.left, xy.top, strBuffer, (int)_tcslen(strBuffer));

	dc.SelectObject(def_font);
	font.DeleteObject();

	return TRUE;
}
