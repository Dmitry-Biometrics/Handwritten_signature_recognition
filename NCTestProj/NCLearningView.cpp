// NCLearningView.cpp : implementation file
//

#include "stdafx.h"
#include "NCTestProj.h"
#include "NCLearningView.h"

#include "DlgBarEx.h"
#include "ProgressLearnDlg.h"
#include "GetKeyDlg.h"
#include "TestDlg.h"
#include "UserAccountDlg.h"
#include "BodyBurdenDlg.h"
#include "StabilityDlg.h"


#include "../Tools/computation.h"

//////////////////////////////////////////////////////////////////////////
// ������� ��� �������� �������
CString sMessage;
CEvent g_eventStart; // ��������� ������ ����� �� ����������
CEvent g_eventKill;  // ������������� ���������� ������� ������
volatile int g_nCount;
CNCLearningView* CNCLearningView::the_instance;
#define WM_THREADFINISHED  (WM_USER + 2)
//////////////////////////////////////////////////////////////////////////

// CNCLearningView

IMPLEMENT_DYNCREATE(CNCLearningView, CFormView)

CNCLearningView::CNCLearningView()
	: CFormView(CNCLearningView::IDD)
{
	// �������� �������������� ����
	ZeroMemory (mixKey, NCKEY_SIZE);

  ZeroMemory(&si, sizeof(si));

	// ����� ������ ���������� �������
	images = new BNCSIGNIMAGE[NCIMAGE_COUNT];
	imageCount = 0;
	ZeroMemory(weightsArr, sizeof(weightsArr));

	// ��� ������ ��������� ���������� ������
	oneImg = new BNCSIGNIMAGE[1];
	oneImg[0].m_X = new UINT [NCMAXCOORD_QUAN];
	oneImg[0].m_Y = new UINT [NCMAXCOORD_QUAN];
	oneImg[0].m_P = new UINT [NCMAXCOORD_QUAN];
	oneImg[0].m_T = new UINT [NCMAXCOORD_QUAN];

	// ��� ������ ������� ���������� �������������� ������
	fixImg = new BNCSIGNIMAGE[1];
	fixImg[0].m_X = new UINT [NCMAXCOORD_QUAN];
	fixImg[0].m_Y = new UINT [NCMAXCOORD_QUAN];
	fixImg[0].m_P = new UINT [NCMAXCOORD_QUAN];
	fixImg[0].m_T = new UINT [NCMAXCOORD_QUAN];
}

CNCLearningView::~CNCLearningView()
{
	// �������� ���������� �������
	delete m_objClientDC;
	
	// �������� ���� ���������� �������
	if ( images != NULL )	// �.�. ���� ������ ���������� ������� ������
	{
		// ������� ���������� ���� ���������� �������
		for (unsigned iCount = 0; iCount < imageCount; iCount++ ) {
			delete [] images[iCount].m_X;
			delete [] images[iCount].m_Y;
			delete [] images[iCount].m_P;
			delete [] images[iCount].m_T;
		}
		delete [] images; 
	}

	// �������� "����������" ����������� ������
	if ( oneImg != NULL ) {
		delete [] oneImg[0].m_X;
		delete [] oneImg[0].m_Y;
		delete [] oneImg[0].m_P;
		delete [] oneImg[0].m_T;
		delete [] oneImg; 
	}

	// �������� "����������" ����������� ������
	if ( fixImg != NULL ) {
		delete [] fixImg[0].m_X;
		delete [] fixImg[0].m_Y;
		delete [] fixImg[0].m_P;
		delete [] fixImg[0].m_T;
		delete [] fixImg; 
	}
}

void CNCLearningView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CNCLearningView, CFormView)
	ON_MESSAGE(WM_THREADFINISHED, OnThreadFinished)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_CLEAR_BTN,	 &CNCLearningView::ClearInputArea)
	ON_BN_CLICKED(IDC_CHECK_BTN,	 &CNCLearningView::CheckImage)
	ON_BN_CLICKED(IDC_DELIMAGE_BTN,  &CNCLearningView::DelImage)
	ON_BN_CLICKED(IDC_CLEARLIST_BTN, &CNCLearningView::DelAllImages)
	ON_BN_CLICKED(IDC_ADD_BTN,		 &CNCLearningView::AddImage)
	ON_BN_CLICKED(IDC_LEARN_BTN,	 &CNCLearningView::LearnNet)
	ON_LBN_SELCHANGE(IDC_IMAGE_LIST, &CNCLearningView::OnShowImage)
	ON_BN_CLICKED(IDC_SECRET_RADIO, &CNCLearningView::OnSecretBtn)
	ON_BN_CLICKED(IDC_PUBLIC_RADIO, &CNCLearningView::OnPublicBtn)
	ON_COMMAND(ID_SAVE_IMAGES,		&CNCLearningView::SaveImages)
	ON_COMMAND(ID_LOAD_IMAGES,		&CNCLearningView::LoadImages)
	ON_COMMAND(ID_CHECK_ALL,		&CNCLearningView::CheckAllImages)
	ON_COMMAND(ID_OBVODKA,			&CNCLearningView::OnObvodkaReg)
	ON_COMMAND(ID_TESTON_TI,		&CNCLearningView::OnTestOnTI)
	ON_COMMAND(ID_TESTON_WN,		&CNCLearningView::OnTestOnWN)
	ON_UPDATE_COMMAND_UI(ID_OBVODKA, &CNCLearningView::OnUpdateObvodka)
//	ON_COMMAND(ID_LOAD_BDTI, &CNCLearningView::OnLoadTIfromBDi)
	ON_UPDATE_COMMAND_UI(ID_CHECK_ALL,	 &CNCLearningView::OnUpdateCheckAll)
	ON_UPDATE_COMMAND_UI(ID_SAVE_IMAGES, &CNCLearningView::OnUpdateSaveImages)
	ON_UPDATE_COMMAND_UI(ID_LOAD_IMAGES, &CNCLearningView::OnUpdateLoadImages)
	ON_COMMAND(ID_SET_PSW,  &CNCLearningView::OnSetPswM)
	ON_COMMAND(ID_PROVERKA, &CNCLearningView::OnProverkaList)
	ON_UPDATE_COMMAND_UI(ID_PROVERKA, &CNCLearningView::OnUpdateProverka)
	ON_COMMAND(ID_BURDEN,	 &CNCLearningView::OnBodyBurdenReg)
	ON_COMMAND(ID_CRACKING,  &CNCLearningView::OnCracking)
	ON_COMMAND(ID_STABILITY, &CNCLearningView::OnStability)
	ON_COMMAND(ID_TI_MULTIPLICATION, &CNCLearningView::OnTIMultiplication)
	ON_WM_TIMER()
	ON_UPDATE_COMMAND_UI(ID_TI_MULTIPLICATION, &CNCLearningView::OnUpdateTIMultiplication)
	ON_COMMAND(ID_MORFING, &CNCLearningView::OnMorfing)
	ON_COMMAND(ID_GRWIDTH, &CNCLearningView::OnGrWidth)
END_MESSAGE_MAP()

void CNCLearningView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
//////////////////////////////////////////////////////////////////////////
	///....
	CNCLearningView::the_instance=this;
	//...
//////////////////////////////////////////////////////////////////////////
	// �������� ���������� �������
	m_objClientDC = new CClientDC(this);
	// ������� � �������� ����	
	mypen = CreatePen( PS_SOLID, 2, RGB(5,3,86));
	m_objClientDC->SelectObject(mypen);

	bSecret       = false;		// �� ��������� �������� ������ "�������"
	oneImg[0].m_Num = 0;	    // ��������� �������� ���������
	bObvodkaOn	  = false;	  // �� ��������� ����� ������� ��������
	bFastModOn	  = false;	  // �� ��������� ����� �������� ������������� ��������
	bLButDown	    = false;	  // �� ��������� ����� ������ ����� �� ������
	bSlideShowOn  = false;	  // �� ��������� ����� �����-��� ��������
  bErr          = false;

	// ���� ������ ����� ��������
	if ( m_wndRightBar.mode == NCTRAIN_MODE ) {
		// ���������, ��� ����� �������� ���� �� ����������
		bLearnNet = FALSE;		
	} else {	// �� ���� ������ ����� ������������
		m_wndRightBar.iLight = 0;		// ������� �������� ��������
	}

  // ������ ������ ��������� ������
  objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\index.ini");
  if ( objFileManip.LoadIniData(objFileManip.tofileFullPath,si) != ERROR_SUCCESS ) {
    MessageBox(_T("����������� ���� � ������."), _T("��������� ���� �� �����"), MB_ICONWARNING);
    bErr = true;
    return;
  }

	// ��������� �������������� ��������� (���� � ���)
	objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si.userName + _T("\\Sign.bnc");
	if ( objFileManip.LoadBNC(objFileManip.tofileFullPath, weightsArr, hashKey ) != ERROR_SUCCESS ) {
		MessageBox(_T("����������� ������������.\n���������� ������� ����."), _T("���� �� �������"), MB_ICONWARNING);
	}

}

// CNCLearningView diagnostics

#ifdef _DEBUG
void CNCLearningView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CNCLearningView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

// CNCLearningView message handlers

// ********************************** ��������� ********************************* //
void CNCLearningView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	// �����
	CPen BluePen;
	CPen RedPen;

	BluePen.CreatePen(  PS_SOLID, 1, RGB(139,136,247) );
	RedPen.CreatePen (  PS_SOLID, 1, RGB(253,126,104) );

	// �������� ������� ������� ���������
	RECT rc;
	GetClientRect( &rc );

	// ������ ����� ����
	dc.SelectStockObject( WHITE_PEN );
//	dc.SelectObject(BluePen);
	dc.Rectangle(rc.left, rc.top, rc.right, rc.bottom);

	dc.SelectObject( BluePen );
	// ������ �������������� �����
	int i;
	for (int step = 48, i = rc.top + step; i <= rc.bottom; step = (step == 48) ? 24 : 48, i += step)
	{
		dc.MoveTo( rc.left + 1,  i );
		dc.LineTo( rc.right - 1, i );
	}
	// ������ ��������� �����
	int dx = (int)(0.4663 * (rc.bottom - rc.top - 2));
  for (i = 82; i < rc.right - rc.left + dx - 1; i += 162){
    dc.MoveTo( i, rc.top + 1 );
    dc.LineTo( i-dx, rc.bottom - 1 );
  }
	dc.SelectObject( RedPen );
	// ������� ���� 
	dc.MoveTo( rc.right - 70, rc.top + 1);
	dc.LineTo( rc.right - 70, rc.bottom - 1);
	// ����� �����

	// �������������� ���������� �����
	if ( oneImg[0].m_Num > 0 && !bSecret ) 
	{
		int x, y;
		x = oneImg[0].m_X[0];
		y = oneImg[0].m_Y[0];
		if ( x != 0 && y != 0 ) m_objClientDC->MoveTo ( x, y );
		else					m_objClientDC->MoveTo ( oneImg[0].m_X[1], oneImg[0].m_Y[1] );

		for ( unsigned iCount = 1; iCount < oneImg[0].m_Num; iCount++ ) {
			if ( oneImg[0].m_X[iCount] != 0 && oneImg[0].m_Y[iCount] != 0 ) 
				m_objClientDC->LineTo ( oneImg[0].m_X[iCount], oneImg[0].m_Y[iCount] );
			else 
				m_objClientDC->MoveTo ( oneImg[0].m_X[iCount+1], oneImg[0].m_Y[iCount+1] );
		}
	}

	// ����� ����� ��������� ���������
	CFont font;
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));       
	lf.lfHeight = 16;      
	lf.lfWeight = FW_NORMAL;
	wcscpy_s(lf.lfFaceName,  _T("MS Sans Serif"));
	VERIFY(font.CreateFontIndirect(&lf));  
	dc.SelectObject(&font);
	// ����� ���� ������
	dc.SetBkMode(TRANSPARENT);
	// ����� ����� ������
	dc.SetTextColor(RGB(255, 0, 0));
	
  dc.TextOut(rc.left+10, rc.bottom-20, sMessage, (int)wcslen(sMessage));

	TCHAR strBuffer[40];	// ��������� �� ����� ���������

	// ���� ������� ����� �������, ������� ��������� �� �����
	if ( bObvodkaOn )
	{
		// ��������� "����������" ������
		CPen *objPen;
		objPen = new CPen;
		objPen->CreatePen(PS_SOLID, 1, RGB(200,0,0));
		m_objClientDC->SelectObject(objPen);
		m_objClientDC->MoveTo ( fixImg[0].m_X[0], fixImg[0].m_Y[0] );
		for ( unsigned iCount = 1; iCount < fixImg[0].m_Num; iCount++ ) {
			if ( fixImg[0].m_X[iCount] != 0 && fixImg[0].m_Y[iCount] != 0 ) 
				m_objClientDC->LineTo ( fixImg[0].m_X[iCount], fixImg[0].m_Y[iCount] );
			else 
				m_objClientDC->MoveTo ( fixImg[0].m_X[iCount+1], fixImg[0].m_Y[iCount+1] );
		}
		objPen->DeleteObject();
		m_objClientDC->SelectObject(mypen);
	
		// ����� ���������
		wcscpy_s(strBuffer, _T("������� ����� �������"));
		dc.TextOut(rc.left+10, rc.bottom-20, strBuffer, (int)wcslen(strBuffer));
	}

	// ���� ������� ����� �������� �������������, ������� ��������� �� �����
	if ( bFastModOn ) {
		wcscpy_s(strBuffer, _T("������� ����� ��������"));	// ����� ���������
		dc.TextOut(rc.left+10, rc.bottom-20, strBuffer, (int)wcslen(strBuffer));
		// ����� �������� ���� ��������
		dc.TextOut(rc.left+10, rc.top+9, cXemMera, (int)wcslen(cXemMera));
	}

//////////////////////////////////////////////////////////////////////////
	// ��������� ������ ��� �������
//	dc.TextOut(rc.left+80, rc.top+115, sMess, (int)wcslen(sMess));
//////////////////////////////////////////////////////////////////////////
	
}

// ������� ����� ������� ����
void CNCLearningView::OnLButtonDown(UINT nFlags, CPoint point)
{
	bLButDown = true; // ��������� �� ������� �����

	// ����������� ��������� ����
	m_objClientDC->MoveTo(point);
	if ( !bSecret ) 
		m_objClientDC->Ellipse(point.x-1, point.y-1, point.x+1, point.y+1);	// ������ �����

	// ���������� ����� ��������� ���� � �������� �������
	beginTime = ::GetTickCount();
	coordCount = 1;			// ��������� �������� ��������� ��������� (������ ������ ���������)

	oneImg[0].m_T[oneImg[0].m_Num] = beginTime;
	oneImg[0].m_X[oneImg[0].m_Num] = point.x;
	oneImg[0].m_Y[oneImg[0].m_Num] = point.y;
	oneImg[0].m_Num++; 

	CFormView::OnLButtonDown(nFlags, point);
}

// ���������� ����� ������� ����
void CNCLearningView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if ( bLButDown )
	{
		bLButDown = false;

		// ���� ����� ������ � ����� ��������� �����(��������� ��� ������),
		// ��������� ������ �������� �������, ������� ��� ��� ����������
		// ���������� ����� ����������� ���������� �������
		oneImg[0].m_T[oneImg[0].m_Num-1] = ::GetTickCount();		// ��������� ����� ������ ����
		beginTime = oneImg[0].m_T[oneImg[0].m_Num-1] - beginTime;	// ���������� ����� ��������� ������ ���������
		UINT lag = beginTime/coordCount;								// ����� �/� ��������� ������������
		for ( unsigned i = 2; i < coordCount; i++ )
			oneImg[0].m_T[oneImg[0].m_Num-i] = 
			oneImg[0].m_T[oneImg[0].m_Num - i + 1] - lag;

		// ������ ��������� �� ������ 
		oneImg[0].m_X[oneImg[0].m_Num] = 0;
		oneImg[0].m_Y[oneImg[0].m_Num] = 0;
		oneImg[0].m_P[oneImg[0].m_Num] = 0;
		oneImg[0].m_T[oneImg[0].m_Num] = 0;
		oneImg[0].m_Num++;
	}

	CFormView::OnLButtonUp(nFlags, point);
}

// �������� ��������� ����
void CNCLearningView::OnMouseMove(UINT nFlags, CPoint point)
{
	// ���� ��� ��������� ������ ����� �� ������� ������� ����� (���� ������������� ������)
	if ( nFlags != MK_LBUTTON && bLButDown  )
	{
		bLButDown = false;
		MessageBox(_T("����� �� ������� ���� �����!"), _T("������ �����"), MB_ICONWARNING);
		ClearInputArea();
		return;
	}

	// ���� ������ ����� �������
	if( nFlags == MK_LBUTTON && bLButDown )
	{
		// ������ ��������� ����������� ������ 		
		if ( oneImg[0].m_Num < NCMAXCOORD_QUAN ) {
			oneImg[0].m_X[oneImg[0].m_Num] = point.x;
			oneImg[0].m_Y[oneImg[0].m_Num] = point.y;
			// ��������� �����
			if ( !bSecret ) {
				m_objClientDC->LineTo(point.x, point.y);
			}
			// ������� ���������� ����������� ���������
			oneImg[0].m_Num++;
			coordCount++;
		} else {
			MessageBox(_T("����� ��������� ����������� ������\n��������� ������������� �����!"), _T("������� ������� �����"), MB_ICONWARNING);
			ClearInputArea();
		}
	}

	CFormView::OnMouseMove(nFlags, point);
}

// �������� ������� �����
void CNCLearningView::ClearInputArea()
{
	oneImg[0].m_Num = 0;			// ��������� �������� ���������
	ZeroMemory(oneImg[0].m_X, NCMAXCOORD_QUAN*sizeof(UINT)); // ��������� ������� ���������
	ZeroMemory(oneImg[0].m_Y, NCMAXCOORD_QUAN*sizeof(UINT)); // ��������� ������� ���������
	ZeroMemory(oneImg[0].m_P, NCMAXCOORD_QUAN*sizeof(UINT)); // ��������� ������� ��������
	ZeroMemory(oneImg[0].m_T, NCMAXCOORD_QUAN*sizeof(UINT)); // ��������� ������� �������� �������
	cXemMera = _T("");				// ������������ � ������ ��������
	RedrawWindow();					// ������������ �������

	// ����� ���������
//	m_wndBotDlgBar.m_ClearBtn.SetCheck(BST_CHECKED);
//	m_wndRightBar.m_AddImageBtn.SetCursor( ::LoadCursor(NULL, IDC_APPSTARTING) );
//	UINT i = m_wndBotDlgBar.m_ClearBtn.GetState();
//	m_wndBotDlgBar.m_ClearBtn.SetState( !(m_wndBotDlgBar.m_ClearBtn.GetState() & 0x0008) );

}

// ******************************* ����� ��������� ****************************** //

//
bool CNCLearningView::isReady()
{
  return true;
}
// �������� ���������� ������ � ������ ��������/������������
void CNCLearningView::CheckImage()
{
  if (bErr)
    return;

	// �������� ���������� ���������
	if( oneImg[0].m_Num == 0 ) {
		MessageBox(_T("You must enter a handwritten image!"),_T("Enter the image"), MB_ICONSTOP);
		RedrawWindow();
		return;
	}
	// �������� ������� �����
	if ( weightsArr[0] == 0.0f ) {
		MessageBox(_T("����������� ���� �����.\n������� ���������� ������� ����."), _T("���� �� �������"), MB_ICONWARNING);
		return;
	}

	// ������������� ���� �� ���������� ������, ���������� ���������� ������������
	int errorCount;								// ���������� ������������

	// �������� ��������� ������ � ������ ������������
	if ( m_wndRightBar.mode == NCTEST_MODE )
	{

		// ��������������� ���������� ����� � ���� � 
		// ������������ ���������� ������� ��� ���������
    objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si.userName + _T("\\");
		int yellowSvet =
						BncExtractKey_2 ( objFileManip.tofileFullPath, oneImg[0], weightsArr, mixKey, checkKey );

		// ����������� ����������� �����
		NCHASH checkHash;
		BncCreateHash ( checkKey, sizeof(NCKEY), checkHash );

		// ��������� ���� ��������� ����� � ����������� 
		BOOL bEqual = BncCompareHash ( hashKey, checkHash );

//////////////////////////////////////////////////////////////////////////
    objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si.userName + _T("\\testKeys.txt");
    objFileManip.SaveKeys(objFileManip.tofileFullPath, checkKey, _T("a"));
//////////////////////////////////////////////////////////////////////////

		if ( bEqual ) {		// ���� ����� ���������� �����
			// ���������� ������ ����
			m_wndRightBar.iLight = 3;
			m_wndRightBar.RedrawWindow();
			MessageBox(_T("Correct image entered"),_T("Result of checking"), MB_ICONASTERISK | MB_OK);
			// ��������� ��������
			m_wndRightBar.iLight = 0;
			m_wndRightBar.RedrawWindow();
		} else {
			// �.�. ���� ����� 90% ������� ������ � ��������,
			// �� ���������� ����� ����, ����� �������
			if ( yellowSvet > NEURON_COUNT*0.9 ) {
				// ���������� ������ ����
				m_wndRightBar.iLight = 2;
				m_wndRightBar.RedrawWindow();
				MessageBox(_T("The entered image is close to the correct one \n Please try again"),_T("Result of checking"), MB_ICONWARNING | MB_OK);
				// ��������� ��������
				m_wndRightBar.iLight = 0;
				m_wndRightBar.RedrawWindow();
			} else {
				// ���������� ������� ����
				m_wndRightBar.iLight = 1;
				m_wndRightBar.RedrawWindow();
				MessageBox(_T("Wrong image entered"),_T("Result of checking"), MB_ICONWARNING | MB_OK);
				// ��������� ��������
				m_wndRightBar.iLight = 0;
				m_wndRightBar.RedrawWindow();
			}
		}
		ClearInputArea();
	}
	
	// �������� ��������� ������ � ������ ��������
	if ( m_wndRightBar.mode == NCTRAIN_MODE )
	{
		// ��������������� ���������� ����� � ����
    objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si.userName + _T("\\");
		BncExtractKey_2 ( objFileManip.tofileFullPath, oneImg[0], weightsArr, mixKey, checkKey );
//////////////////////////////////////////////////////////////////////////
    objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si.userName + _T("\\testKeys.txt");
    objFileManip.SaveKeys(objFileManip.tofileFullPath, checkKey, _T("a"));
//////////////////////////////////////////////////////////////////////////
		// ���������� ���������� ���� � ���������
		errorCount = BncCompareKeys( si.key, checkKey );

		if ( errorCount == 0 ) {	
			CGetKeyDlg objGetKeyDlg(NCTRAIN_MODE, TRUE, FALSE, &si.key, &checkKey, NULL);
			objGetKeyDlg.DoModal();
			ClearInputArea();
		} else  {
			CGetKeyDlg objGetKeyDlg(NCTRAIN_MODE, FALSE, FALSE, &si.key, &checkKey, NULL);
			objGetKeyDlg.DoModal();

			// ���� ����� ������ � ��������� �������, ���������� ������������
			// �������� ����� � ��������� ������� � ����������� ����
			if ( errorCount < NEURON_COUNT*1 ) {
				CFont font;
				LOGFONT lf;
				TCHAR strBuffer[51];
				memset(&lf, 0, sizeof(LOGFONT));       
				lf.lfHeight = 16;      
				lf.lfWeight = FW_BOLD;
				wcscpy_s(lf.lfFaceName,  _T("MS Sans Serif"));
				VERIFY(font.CreateFontIndirect(&lf));  
				CClientDC dc(this);
				dc.SelectObject(&font);
				wcscpy_s(strBuffer, _T("���� �� ������, ����� ��������� ����� ��� ��������"));
				dc.TextOut(20, 42, strBuffer, (int)wcslen(strBuffer));
				wcscpy_s(strBuffer, _T("������� \"��������\", � ����� \"�������\""));
				dc.TextOut(50, 65, strBuffer, (int)wcslen(strBuffer));
			}
			else ClearInputArea();
		}
	}
}

// ������� ���������� ����� (�� ������)
void CNCLearningView::DelImage()
{
	// ����� ���������� ������
	int iCheckedImage = m_wndRightBar.m_ImageList.GetCurSel();

	if( iCheckedImage != -1 )
	{
		if ( MessageBox ( _T("�� �������, ��� ������ ������� ��������� �����?"), _T("�������� ���������� ������"), MB_ICONQUESTION|MB_YESNO) == IDYES )
		{
			objFileManip.DelImageCoord( iCheckedImage, imageCount-1, images );
			
			ClearInputArea();
			imageCount--;
								
			// ��������� ���������� ������������ ��������� �������
			m_wndRightBar.m_ImageList.DeleteString(imageCount);
			m_wndRightBar.m_ImageList.SetCurSel(imageCount-1);

//			MessageBox ( _T("��������� ����� ������� ������."), _T(""), MB_ICONASTERISK);
		}
	} else {
		MessageBox ( _T("��� ���������� ������."), _T("����� �� ������"), MB_ICONASTERISK);
	} // ����� �������� ������		

}

// ������� ��� ������ (�������� ������)
void CNCLearningView::DelAllImages()
{
	if ( imageCount == 0 ) {
		MessageBox( _T("������ ����"), _T(""), MB_ICONASTERISK );
		return;
	}
	if ( MessageBox ( _T("�� �������, ��� ������ ������� ��� ����������� ������?"), _T("�������� ��������� ���������"), MB_ICONQUESTION|MB_YESNO) == IDYES )
	{
		// ������� ���������� ���� ���������� �������
		objFileManip.DelAllImages ( imageCount, images );		
		m_wndRightBar.m_ImageList.ResetContent();
		ClearInputArea();
		//MessageBox ( _T("��� ������ ������� �������."), _T(""), MB_ICONASTERISK);
	}
}

// ���������� ������ ������
void CNCLearningView::AddImage()
{
	// �������� ���������� ���������
	if( oneImg[0].m_Num == 0 ) {
		MessageBox(_T("������ ��������� ������ �����!"),_T("��� ������"), MB_ICONSTOP);
		RedrawWindow();
		return;
	}

	if ( imageCount < NCIMAGE_COUNT ) 
	{
		if ( (oneImg[0].m_T[oneImg[0].m_Num-2] - oneImg[0].m_T[0]) < 0 ) {
			MessageBox(_T("������ ����� �� ����� ���� �������� � �������"),_T("�������� �����"), MB_ICONSTOP);
			RedrawWindow();
			return;
		}

		// ������� �������� ���������� � ������ ���������
		images[imageCount].m_Num = oneImg[0].m_Num;
		images[imageCount].m_X = new UINT [oneImg[0].m_Num];
		images[imageCount].m_Y = new UINT [oneImg[0].m_Num];
		images[imageCount].m_P = new UINT [oneImg[0].m_Num];
		images[imageCount].m_T = new UINT [oneImg[0].m_Num];
		for ( unsigned i = 0; i < oneImg[0].m_Num; i++ )	{
			images[imageCount].m_X[i] = oneImg[0].m_X[i];
			images[imageCount].m_Y[i] = oneImg[0].m_Y[i];
			images[imageCount].m_P[i] = oneImg[0].m_P[i];
			images[imageCount].m_T[i] = oneImg[0].m_T[i];
		}
		// ���������� �������� �������
		imageCount++;
		// ����������� ������ ���������� �������
		CString str;
		str.Format(_T("     %d"), imageCount);
		m_wndRightBar.m_ImageList.InsertString(imageCount-1, str);
		m_wndRightBar.m_ImageList.SetCurSel(imageCount-1);
	}
	else MessageBox( _T("���������� ������ ����������!\n���������� ������������ ����������\n�������������� �������� �������!"), _T("������ ����������"), MB_ICONWARNING);

	// ������� ������� �����	
	ClearInputArea();
}

// �������� ��������� ����
void CNCLearningView::LearnNet()
{
  if (bErr)
    return;

	// �������� ������������ ������� �������
	if ( imageCount > 2 )
	{
		// �������� ��������� ����
		CProgressDlg objDlg ( imageCount, images, &si, &mixKey, &weightsArr, NULL );
		objDlg.DoModal();
		bLearnNet = TRUE;
//////////////////////////////////////////////////////////////////////////
    // ��������� ��������� ����
    objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si.userName + _T("\\trainKey.txt");
    objFileManip.SaveKeys(objFileManip.tofileFullPath, si.key, _T("w"));
    // ��������� ����
    objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si.userName + _T("\\weights.txt");
    objFileManip.SaveCoefs(objFileManip.tofileFullPath, "������� ������� �������������:\n", weightsArr, sizeof(NCWEIGHTS)/sizeof(weightsArr[0]), _T("w"));
//////////////////////////////////////////////////////////////////////////
	}
	else
		MessageBox( _T("������������ ������� ��� ��������"), _T("�������� ��������� �������"), MB_ICONASTERISK );
}

// ��������� �������� ������ �������
void CNCLearningView::OnShowImage()
{
	ClearInputArea();

	// ����� ���������� ������
	int iCheckedImage = m_wndRightBar.m_ImageList.GetCurSel();

	if ( iCheckedImage == -1) return;	// �.�. � ���� ��������, � ����� �� �������

	// ������ ��������� ���������� ������
	if ( bObvodkaOn ) {		// ���� ����� ������� �������
		fixImg[0].m_Num = images[iCheckedImage].m_Num;
		memcpy ( fixImg[0].m_X, images[iCheckedImage].m_X, fixImg[0].m_Num*sizeof(UINT) );
		memcpy ( fixImg[0].m_Y, images[iCheckedImage].m_Y, fixImg[0].m_Num*sizeof(UINT) );
		memcpy ( fixImg[0].m_P, images[iCheckedImage].m_P, fixImg[0].m_Num*sizeof(UINT) );
		memcpy ( fixImg[0].m_T, images[iCheckedImage].m_T, fixImg[0].m_Num*sizeof(UINT) );

	} else {				// ���� �� ������������ �������
		oneImg[0].m_Num = images[iCheckedImage].m_Num;
		memcpy ( oneImg[0].m_X, images[iCheckedImage].m_X, oneImg[0].m_Num*sizeof(UINT) );
		memcpy ( oneImg[0].m_Y, images[iCheckedImage].m_Y, oneImg[0].m_Num*sizeof(UINT) );
		memcpy ( oneImg[0].m_P, images[iCheckedImage].m_P, oneImg[0].m_Num*sizeof(UINT) );
		memcpy ( oneImg[0].m_T, images[iCheckedImage].m_T, oneImg[0].m_Num*sizeof(UINT) );
	}

	// ���� ������� ����� ������� ��������, ��������� ���� ��������
	if ( bFastModOn )
	{
		// ��������������� ���������� ����� � ����
		BncExtractKey ( oneImg[0], weightsArr, mixKey, checkKey );
		// ���������� ���������� ���� � ���������
		int errCount = BncCompareKeys( si.key, checkKey );
		cXemMera.Format(_T("���� �������� : %d"),errCount);
	}

	RedrawWindow();	// ������������ �������

}

// ������ ����� ������
void CNCLearningView::OnSecretBtn()
{
	bSecret = true;
	ClearInputArea();
}


// �������� ����� ������
void CNCLearningView::OnPublicBtn()
{
	bSecret = false;
	ClearInputArea();
}

// ����� �������� ����������������� ������
void CNCLearningView::OnSetPswM()
{
	// ���� ������ ����� ��������
	if ( m_wndRightBar.mode != NCTRAIN_MODE )
    return;

  CUserAccountDlg objDlg;
	INT_PTR nResponse = objDlg.DoModal();
	if ( nResponse != IDOK )
    return;

  // ���������, ��� ����� �������� ���� �� ����������
	bLearnNet = FALSE;		
	// ��������� ����� ��������� ����
  objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\index.ini");
  if ( objFileManip.LoadIniData(objFileManip.tofileFullPath,si) != ERROR_SUCCESS ) {
    MessageBox(_T("������ ������ ���������� �����."), _T("��������� ���� �� �����"), MB_ICONWARNING);
    bErr = true;
  }
  bErr = false;
}

// ������ ���� ���� ���������� � ������ ������������ 
// � �������� � ������ ��������
void EnableMyMenu ( CCmdUI *pCmdUI )
{
	if ( m_wndRightBar.mode == NCTEST_MODE ) 
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

// ��������� ��� ��������� ������ �� ����
void CNCLearningView::SaveImages()
{
	// ���� ��������� ������ ����������, �� ��������� �� � ����
	if ( imageCount != 0 ) {
		TCHAR szFilters[] = _T("Image Files (*.dat)|*.dat|All Files (*.*)|*.*||");
		CFileDialog fileOpenDlg (FALSE, _T("my"), _T("MyImages.dat"),
			OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);

		if ( fileOpenDlg.DoModal () == IDOK ) {
			CString str = fileOpenDlg.GetPathName();	// ���� �� �������� ������� �������� ��������� ������
			WCHAR szPath[MAX_PATH];
			lstrcpy(szPath, str);
			long lErr = objFileManip.SaveAllImages( szPath, imageCount, images );
			//images
		
			
			// ��������� ������� �� ���� � ���������� ��������� �� ����
			if ( lErr == ERROR_SUCCESS ) 
				str = _T("��� ��������� ������� ������� ���������!");
			else 
				str = _T("��������� �� ������ ��������� ��� ��������� �������.");

			MessageBox(str, _T("���������� �������"), MB_ICONASTERISK);
		}

	}
	else
		MessageBox(_T("� ��������� ������� ��� �� ������ �������."), _T("��������� ������� �����."), MB_ICONASTERISK);

}
void CNCLearningView::OnUpdateSaveImages(CCmdUI *pCmdUI)
{
	EnableMyMenu(pCmdUI);
}

// ��������� ��������� ������ � �����
void CNCLearningView::LoadImages()
{
	TCHAR szFilters[] = _T("Image Files (*.dat)|*.dat|All Files (*.*)|*.*||");
	CFileDialog fileOpenDlg (TRUE, _T("my"), _T("MyImages.dat"),
		OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);
		
	if ( fileOpenDlg.DoModal() == IDOK ) {
		CString str = fileOpenDlg.GetPathName();	    // ���� � ����� �� �������
		objFileManip.DelAllImages(imageCount, images);  // ������� ��� ������ ��������� ������
		imageCount = 0;

		// ��������� ����� ��������� ������
		long lErr =	objFileManip.LoadAllImages(str, imageCount, images);

		// ���� ��� ������ ������� ���������, �� ������� ��������������� ���������
		if( lErr == ERROR_SUCCESS )
			str.Format(_T("��������� ������ ��������� %d �������."), imageCount);
		else
			str = " ��������� �� ������ ��������� ��� ��������� �������.";

		// ����� ��������� � ��������\��������� ��������
		MessageBox(str, _T("������ ��������� ��������"), MB_ICONASTERISK);

		// ������� ������� �����
		ClearInputArea();

		// ����������� ������ ���������� �������
		m_wndRightBar.m_ImageList.ResetContent();
		for( unsigned iCount = 0; iCount < imageCount; iCount++) {
			str.Format(_T("     %d"), iCount+1);
			m_wndRightBar.m_ImageList.InsertString(iCount,str);
		}	
	}
}

void CNCLearningView::OnUpdateLoadImages(CCmdUI *pCmdUI)
{
	EnableMyMenu(pCmdUI);
}

// ��������� ������������� (��������� ��� ������ �� ������)
void CNCLearningView::CheckAllImages()
{
	ClearInputArea();

	// ���� � ������ ���� ������, �� ���������, ����� - ���
	if ( imageCount == 0 ) {
		MessageBox(_T("��� ���������� ��������� �������� � ������\n������ ����������� ���� �� ���� �����."), _T("������ ����"), MB_ICONASTERISK);
		return;
	}

	int count = 0;	// ���������� ������� ������
	// ��������������� ��������� ���� �������� �� ���� ������� �� ������
	for ( unsigned i = 0; i < imageCount; i++ )
	{
		// ����������� ����� � ����
		BncExtractKey ( images[i], weightsArr, mixKey, checkKey );
		// ���������� �������� ���� � ����������
		int mera = BncCompareKeys ( si.key, checkKey );
		if ( mera == 0 ) count++;
	}

	// ����� ����������� �������� �� �����
	CString str;
	str.Format(_T("����� ��������� ������� : %d"), imageCount);
	m_objClientDC->ExtTextOut(5,5,ETO_CLIPPED, NULL, str, (UINT)wcslen(str), NULL);
	str = _T("���������� �������� : ");
	m_objClientDC->ExtTextOut(5,25,ETO_CLIPPED, NULL, str, (UINT)wcslen(str), NULL);
	str.Format(_T("���������� ������� \"������\" : %d. ���������� ������� \"������\" : %d."), count, imageCount-count);
	m_objClientDC->ExtTextOut(5,40,ETO_CLIPPED, NULL, str, (UINT)wcslen(str), NULL);

/*
	ClearInputArea();

	CConversionSFN objSFN;
	CSimNet objSim;
	// ������ ��� ������������ ����� ���� ���������� �������
	float *coefficientsArr = new float [imageCount*NCFOURIER_COEF];
	objSFN.NcCalcImagesCoef ( imageCount, images, coefficientsArr );

	// ���������� ������ ���� �� ���������� �������������
//////////////////////////////////////////////////////////////////////////
	int xemArr[100];
	objSim.NetRecognition(imageCount,coefficientsArr, weightsArr, binKeyArr,xemArr);
	float aver = 0;
	for (int i =0; i<imageCount; i++) {
		aver += xemArr[i]; 
	}
	aver /= imageCount;
//////////////////////////////////////////////////////////////////////////
	int errArr[2] = {0};
	objSim.GroupSimulationForLayerCompare (
		imageCount, coefficientsArr, weightsArr, binKeyArr, errArr );
	delete [] coefficientsArr;
	CString str;
	str.Format(_T("����� ��������� ������� : %d"), imageCount);
	m_objClientDC->ExtTextOut(5,5,ETO_CLIPPED, NULL, str, (UINT)wcslen(str), NULL);
	str = _T("���������� �������� �� ������ ���� : ");
	m_objClientDC->ExtTextOut(5,25,ETO_CLIPPED, NULL, str, (UINT)wcslen(str), NULL);
	str.Format(_T("���������� ������� \"������\" : %d. ���������� ������� \"������\" : %d."), errArr[0], imageCount-errArr[0]);
	m_objClientDC->ExtTextOut(5,40,ETO_CLIPPED, NULL, str, (UINT)wcslen(str), NULL);
	str = _T("���������� �������� �� ������ ���� : ");
	m_objClientDC->ExtTextOut(5,60,ETO_CLIPPED, NULL, str, (UINT)wcslen(str), NULL);
	str.Format(_T("���������� ������� \"������\" : %d. ���������� ������� \"������\" : %d."), errArr[1], imageCount-errArr[1]);
	m_objClientDC->ExtTextOut(5,75,ETO_CLIPPED, NULL, str, (UINT)wcslen(str), NULL);
*/	
}
void CNCLearningView::OnUpdateCheckAll(CCmdUI *pCmdUI)
{
	EnableMyMenu(pCmdUI);
}

// � ������� ���� �������� ������� ������� ������� ��������� ����
void CNCLearningView::OnCracking()
{
//////////////////////////////////////////////////////////////////////////
// ����� �� ������ �����
  return;
//////////////////////////////////////////////////////////////////////////


	LONG lErr = ERROR_SUCCESS;

	TCHAR szFilters[] = _T("Image Files (*.dat)|*.dat|All Files (*.*)|*.*||");
	CFileDialog fileOpenDlg (TRUE, _T("my"), NULL,
		OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);

	if ( fileOpenDlg.DoModal() != IDOK ) return;
	
	CString path = fileOpenDlg.GetPathName();	    // ���� � ����� �� �������
	CString name = fileOpenDlg.GetFileName();		// ��� �����
	CString folder;									// ����� � �����������

	// ���������� ���� � ����� � �������
	folder = path.Left( path.Find(name, 0) );

	// ��������� ������ �� ������� ����� ��������� ����
	BNCSIGNIMAGE *tstImg;   
	tstImg = new BNCSIGNIMAGE[NCIMAGE_COUNT];
	UINT count = 0;			// ���������� ������� � ����� �����
	UINT totalCount = 0;	// ����� ���������� �������
	int	sumKey[NEURON_COUNT] = {0}; // ����� ��������� ���� ������
	int keyArr[NEURON_COUNT] = {0}; 
	int crKey[NEURON_COUNT]  = {0};

	// ��������� ���������� �������� �������
	WIN32_FIND_DATA FindFileData;
	wchar_t cPath[MAX_PATH];
	wcscpy_s ( cPath, folder );
	wcsncat_s ( cPath, _T("*.dat"), 5 );
	// ���������� ��� ����� � ������������
	HANDLE hFind = FindFirstFile( cPath, &FindFileData );
	if ( hFind != INVALID_HANDLE_VALUE ) 
	{
		// ���������������� ������������ �� ������� �� ���� ������
		do 
		{	
			// ��������� �������� ������
			count = 0;			
			CString str		= FindFileData.cFileName;
			CString toCoord = folder + str;
			lErr = objFileManip.LoadAllImages(toCoord, count, tstImg);
			if ( lErr != ERROR_SUCCESS ) continue;
			totalCount += count;

			// ���������������� ���������� ������
			for ( unsigned i = 0; i < count; i++ )
			{
				// ����������� ����� � ����
				BncExtractKey ( tstImg[i], weightsArr, mixKey, checkKey );
				BncConvertKey ( checkKey, keyArr );
				for ( int i = 0; i < NEURON_COUNT; i++ ) {
					sumKey[i] += keyArr[i];
				}
			}
			objFileManip.DelAllImages(count, tstImg);

		} while ( FindNextFile(hFind, &FindFileData) != 0 );

		FindClose(hFind);

	}

	// ������������ ���������� ����������

	// �������, ������� ���� ���������� �� ������ ������� �������,
	// ������������� ��� "����������" ���/���
	for ( int i = 0; i < NEURON_COUNT; i++ )  {
		crKey[i] = (sumKey[i] < (int)(totalCount/2)) ? 0 : 1;
	}
	// �������, ������ ������� �� ������� ������� ������������� �� ����,
	// ��������� �������. ��������� �� ����������.
	BncConvertKey ( si.key, keyArr );//�������� ��������� ����
	int goodN = 0;	// ���������� "���������" ��������� ��������
	for ( int i = 0; i < NEURON_COUNT; i++ ) {
		if ( keyArr[i] != crKey[i] )	goodN++;		
	}
	CString str;
	str.Format(_T("���������� ��������� ��������� ��������: %d"), goodN);
 	MessageBox( str, _T(""), MB_OK);

	delete [] tstImg;

}

// �������� ����� ������� (��������� �������� ������ �� ������)
void CNCLearningView::OnObvodkaReg()
{
	// ��������/��������� ����� �������	
	bObvodkaOn ^= true;
	if ( bObvodkaOn ) {
		fixImg[0].m_Num = 0;
		MessageBox(_T("����� ������� �������"));
	}
	else
		MessageBox(_T("����� ������� ��������"));
	
	ClearInputArea();
}
void CNCLearningView::OnUpdateObvodka(CCmdUI *pCmdUI)
{
	EnableMyMenu(pCmdUI);
	
	// ������ ���� ������� �������
	if ( bObvodkaOn )
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}
// ����� ������� �������� ������� 
// ��� ������ ���������� ������ �� ������ ���������
// ��� ����� � ���� ��������
void CNCLearningView::OnProverkaList()
{
	// ��������/��������� ����� ��������	
	bFastModOn ^= true;
	if ( bFastModOn )	MessageBox(_T("����� �������� �������"));
	else				MessageBox(_T("����� �������� ��������"));
	ClearInputArea();
}
void CNCLearningView::OnUpdateProverka(CCmdUI *pCmdUI)
{
	EnableMyMenu(pCmdUI);
	// ������ ���� ������� �������
	if ( bFastModOn )	pCmdUI->SetCheck(1);
	else				pCmdUI->SetCheck(0);
}

// ������� ������������� ��������� ����� �� ������
// ��� ����� ������ ���� �� ������� ��������� �������������, 
// ����� �������� ��������� ���� � �� ���������� ����������� �������� �� ������
void CNCLearningView::OnStability()
{
  //////////////////////////////////////////////////////////////////////////
  // ����� �� ������ �����
  return;
  //////////////////////////////////////////////////////////////////////////

	ClearInputArea();

	TCHAR szFilters[] = _T("Image Files (*.dat)|*.dat|All Files (*.*)|*.*||");
	CFileDialog fileOpenDlg (TRUE, _T("my"), NULL,
		OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);

	if ( fileOpenDlg.DoModal() == IDOK ) 
	{
		CString path = fileOpenDlg.GetPathName();	    // ���� � ����� �� �������
		CString name = fileOpenDlg.GetFileName();		// ��� �����
		CString folder;									// ����� � �����������

		// ���������� ���� � ����� � �������
		folder = path.Left( path.Find(name, 0) );

		CStabilityDlg objDlg ( folder, &si.key, &mixKey );
		objDlg.DoModal();
	}
}
// ����� ������� ������� �������� �� ������������ � ���������
void CNCLearningView::OnBodyBurdenReg()
{
  //////////////////////////////////////////////////////////////////////////
  // ����� �� ������ �����
  return;
  //////////////////////////////////////////////////////////////////////////

	// ������ ������� �������� �� ������������ � ���������
	CBodyBurdenDlg objDlg(&si, &mixKey, NULL);
	objDlg.DoModal();
}


// ��������� ������� (����������� ���������� ������� � ������� ������ ���������� �������
// ��������� � �������� ����� �� ������� �� �����)
// count	- [in] ���������� �������
// allKeys	- [in] �����, ���������� �� count �������� �������
// pos		- [in] ������ ������� � ������� ����� r-�� ������� ������ � ��������� ������
// trainKey - [in] ��������� ����
CString GetCountAndPos( int count, int allKeys[], int pos[], int trainKey[] )
{
	CString str;

	int *tmpKey = new int [count*NEURON_COUNT];
	for ( int i = 0; i < count; i++ ) 
	{
		for ( int j = 0; j < NEURON_COUNT; j++ ) 
		{
			tmpKey[i*NEURON_COUNT+j] = allKeys[pos[i]*NEURON_COUNT+j];
		}
	}
	int r = rand()%256;	// ����� ���������������� �������
	int coin = 0;
	for ( int i = 0; i < count; i++ ) 
	{
		if ( tmpKey[i*NEURON_COUNT + r] == trainKey[r] ) 
		{ 
			pos[coin] = i;
			coin++;
		}
	}
	str.Format(_T("%d\t%d "), count, coin);
	if ( count > 10) {
		str += GetCountAndPos ( coin, tmpKey, pos, trainKey );
	}
	delete [] tmpKey;

	return str;
}

// �������������� ��������� ���� �� �������� �������
void CNCLearningView::OnTestOnTI()
{
//////////////////////////////////////////////////////////////////////////
// ����� ������� ������� ������������
	// ��������� ���������� �����
//	objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\tempKey_all.txt");
//	int totalIMGCount = 10189;
	//objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\tempKeyW.txt");
	//const int totalIMGCount = 100000;
	//int *tempKey = new int [totalIMGCount*NEURON_COUNT];
	//int iCount = totalIMGCount*NEURON_COUNT;
	//objFileManip.LoadXem(objFileManip.tofileFullPath, tempKey, iCount);
	//int trnKey[NEURON_COUNT] = {0}; 
	//BncConvertKey ( trainKey, trnKey );//�������� ��������� ����

	//srand(::GetTickCount());

	//// ���������� ���������� ������� � ������� ����� ���������� ������� ������ � ��������� ������	
	//int pos[totalIMGCount];		// ������ ������� � ������� ����� r-�� ������� ������ � ��������� ������
	//for ( int i = 0; i < totalIMGCount; i++ ) pos[i] = i;
	//CString str = GetCountAndPos ( totalIMGCount, tempKey, pos, trnKey );

	//return;
//////////////////////////////////////////////////////////////////////////

/**/
	ClearInputArea();

	TCHAR szFilters[] = _T("Image Files (*.dat)|*.dat|All Files (*.*)|*.*||");
	CFileDialog fileOpenDlg (TRUE, _T("my"), NULL,
		OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);

	if ( fileOpenDlg.DoModal() == IDOK ) 
	{
		CString path = fileOpenDlg.GetPathName();	    // ���� � ����� �� �������
		CString name = fileOpenDlg.GetFileName();		// ��� �����
		CString	type = fileOpenDlg.GetFileExt();		// ��� �����
		CString folder;									// ����� � �����������

		// ���������� ���� � ����� � �������
		folder = path.Left( path.Find(name, 0) );
				
		CTestDlg objTest ( NCTEST_MODE_TIWN, TRUE, folder, type, &si, &mixKey, &weightsArr );
		objTest.DoModal();
	}
/**/	
/*
	ClearInputArea();

	// ��������� �������� ��� �������� �� �������� �������
	int   tstImageCount = 0;			// ���������� �������� �������

	// ������ �������� �������
	BNCSIGNIMAGE *tstImages;			// ��������� �� ���������� �������� ���������� �������
	tstImages = new BNCSIGNIMAGE[NCIMAGE_COUNT];

	objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\TImages.dat");
	// ��������� ���������� �������� ������� �� ����� �� ���������� ����
	long lErr = 
		objFileManip.LoadAllImages(objFileManip.tofileFullPath, tstImageCount, tstImages);
	// ���� ��� ������ ������� ���������, �� ������� ��������������� ���������
	if( lErr != ERROR_SUCCESS ) {
		MessageBox(_T("�������� ������ �� �������"), _T("������������ ��������!"), MB_ICONWARNING);
		return;
	}
	
	// ��������� ������� ��� �������� �� �������� �������
	BNCMOMENTS params;	// ��������� ��� ��������
	params = BncGetXemMoments ( TRUE, trainKey, mixKey, tstImages, 
								tstImageCount, weightsArr );

	// �������� �������� �������
	for ( int iCount = 0; iCount < tstImageCount; iCount++ ) {
		delete [] tstImages[iCount].points;
		delete [] tstImages[iCount].m_T;
	}
	delete [] tstImages; 
	
	CString str;
	str.Format(_T(" ����������� : %.3f\n ��������� : %.3f\n �������� : %.3f\n"), params.faver, params.fdisp, params.fqual);
	MessageBox(str, _T("������������ ���������!"), MB_ICONASTERISK);
*/
}

// �������������� ��������� ���� �� ����� ����
void CNCLearningView::OnTestOnWN()
{
	ClearInputArea();

//	BNCMOMENTS params;	// ��������� ��� ��������
	CTestDlg objTest ( NCTEST_MODE_TIWN, FALSE, NULL, NULL, &si, &mixKey, &weightsArr );
	objTest.DoModal();
//////////////////////////////////////////////////////////////////////////
	//POINT netOut01[NEURON_COUNT];	// ���-�� ��������� 0 � 1 �� ���� �������� �������
	//ZeroMemory(netOut01, sizeof(netOut01));
	//int i = 0;
	//while ( i < 5000 )
	//{
	//	int count = 20;
	//	params = BncGetXemMoments ( FALSE, trainKey, mixKey, 
	//								NULL, count, weightsArr );
	//	for ( int j = 0; j < NEURON_COUNT; j++ ) {
	//		netOut01[j].x += params.p01[j].x;
	//		netOut01[j].y += params.p01[j].y;
	//	}
	//	//��������� ���������� �����
	//	objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\tempKeyW.txt");
	//	objFileManip.SaveXem(objFileManip.tofileFullPath, params.tmpKey, count*NEURON_COUNT, _T("a"));
	//	i++;
	//}
//////////////////////////////////////////////////////////////////////////
	/*params = BncGetXemMoments ( FALSE, trainKey, mixKey, 
								NULL, 1000, weightsArr );
	// ��������� ��������� ������� � ������ �������
	float fstab = 0.87f+0.221f*params.fqual*params.fqual;

	CString str;
	str.Format(_T("\t����������� : %.3f\n \t��������� : %.3f\n \t�������� : %.3f\n��������� � ������ ������� ����� : 10^%.3f"), params.faver, params.fdisp, params.fqual, fstab);
	MessageBox(str, _T("������������ ���������!"), MB_ICONASTERISK);*/
}

// ������ ����� ��� ������ �����-���
UINT CNCLearningView::ComputeThreadProc(LPVOID pParam)
{
	::WaitForSingleObject(g_eventStart, INFINITE);
	g_nCount = 1;
	THREADDATA *sourceData;		// �������� ������
	sourceData = (THREADDATA*) pParam;
	bool bEnd = false;			// ���������� ������
	while (::WaitForSingleObject(g_eventKill, 0) != WAIT_OBJECT_0)
	{
		if (bEnd) break;
		// "�����������" ������� (����������� sourceData->count �������� ����������
		// ������� � pairCount*(interpCount) �������)
		// ��������, ��� 32 �������� ������� pairCount = 496 (���������� ��������� ���)
		// interpCount = 11 ���������� 5456 �������
		UINT i1, i2;	// ������ ��������������� �������
		int pairCount = 0;		// ���������� ��� �������
		int interpCount = 12;	// ���������� ������������ ������� ����� i1 � i2 (�������� 
								// � ������� �������� ������������ ��������� ���� ���������������
								// �������), �������� ������ ���� ��������, �.�. ���������� 
								// interpCount-2 ����� ������
		for ( i1 = 0; i1 < sourceData->count-1; i1++ ) // ����� ������� ������ � ����
		{
      int tmpCount = 0;
			if (bEnd) break;
			for ( i2 = i1+1; i2 < sourceData->count; i2++ ) // ����� ������� ������ � ����
			{
				if (bEnd) break;
				// ���������� ����� � ����� ������
				int pointCount = 
					(sourceData->img[i1].m_Num < sourceData->img[i2].m_Num) ? 
					sourceData->img[i1].m_Num : sourceData->img[i2].m_Num;
				for ( int i = 0; i < interpCount; i++ )
				{
					if (bEnd) break;
					for ( int j = 0; j < pointCount; j++ )
					{
						the_instance->oneImg[0].m_X[j] = 
							(UINT)(((interpCount-i)/(float)interpCount) * sourceData->img[i1].m_X[j] + 
							(i/(float)interpCount) * sourceData->img[i2].m_X[j]);
						the_instance->oneImg[0].m_Y[j] = 
							(UINT)(((interpCount-i)/(float)interpCount) * sourceData->img[i1].m_Y[j] + 
							((i)/(float)interpCount) * sourceData->img[i2].m_Y[j]);
					}
					the_instance->oneImg[0].m_Num = pointCount;
          tmpCount++;
          sMessage.Format(_T("%d"),tmpCount);
					// ���������������� ����� (��� ����������� ����������� ������)
					g_eventStart.ResetEvent();
					::WaitForSingleObject(g_eventStart, INFINITE);

					if (::WaitForSingleObject(g_eventKill, 0) == WAIT_OBJECT_0)
					{
						bEnd = TRUE;
					}

				}
				pairCount++;
			}
		}
	}
	// �������� ������������� ���� � ���������� ������ ������
	::PostMessage(sourceData->pWnd, WM_THREADFINISHED, 0, 0);
	g_nCount = 0;

	return 0; // ends the thread

}

// ������� ��������� ������ ��� �����-��� ����� ���������� ��������� ������
LONG CNCLearningView::OnThreadFinished(UINT wParam, LONG lParam)
{
	KillTimer(m_nTimer); 
	return 0;
}
// ���������� ���������� �������� ������� �� ���� �������� ����������
// ������������� �� 32 ������� �� ����� minXemImg.dat �������� ������ ����������
// �������� �������, ��������� ��������� �������� �������
void CNCLearningView::OnTIMultiplication()
{
  //////////////////////////////////////////////////////////////////////////
  // ����� �� ������ �����
  return;
  //////////////////////////////////////////////////////////////////////////

// ������ ��������� �� ������������ ������������� ��������� ���� �� ������������ �������!!!
// �������������� ������ ���������� ������������ ��������� ��������

	bSlideShowOn ^= true;

	// ���� ����� ��� �����-��� �������, ������������� ��� ����������
	if (g_nCount != 0)
	{
		g_eventKill.SetEvent();
		return;
	}

  // ��������� �������� ������ 
	if (imageCount != 0) {
		objFileManip.DelAllImages(imageCount, images); 
	}
	// ���������� �� �������� ������ �� ����� Data\\minXemImg.dat, � ��� �������������������
	// ������ �� ����� Data\\AfterScalImg.dat (�.�. ��� ����������� ����� ����� � ������
	// ������ ���� ���������� ���������� �����)
	objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\AfterScalImg2.dat");
	WCHAR szPath[MAX_PATH];
	lstrcpy(szPath, objFileManip.tofileFullPath);
	long lErr =	objFileManip.LoadAllImages(szPath, imageCount, images);
	if ( lErr != ERROR_SUCCESS ) { 
		return;
	}
	// ������� ������� �����
	ClearInputArea();
	// ����������� ������ ���������� �������
	m_wndRightBar.m_ImageList.ResetContent();
	CString str;
	for( unsigned iCount = 0; iCount < imageCount; iCount++) {
		str.Format(_T("     %d"), iCount+1);
		m_wndRightBar.m_ImageList.InsertString(iCount,str);
	}	

	// ������������� ������ ��� ��������� �����-���
	m_nTimer = (int)SetTimer(1, 1000, NULL);//����� ������ �������
 
	// ������� ������ ����� (��� ������ �����-���)
	// ������ ��� �������� ������
	thData.pWnd = GetSafeHwnd();
	thData.count = imageCount;
	thData.img = images;
	m_pRecalcWorkerThread =	AfxBeginThread(ComputeThreadProc, &thData);

	// ��������� ������ ����� �� ����������
	g_eventStart.SetEvent();

}
// ������ ���� ������� ������� ��� ������ �����-���
void CNCLearningView::OnUpdateTIMultiplication(CCmdUI *pCmdUI)
{
	EnableMyMenu(pCmdUI);
	// ������ ���� ������� �������
	if ( bSlideShowOn )	pCmdUI->SetCheck(1);
	else				pCmdUI->SetCheck(0);
}
// � ������ �������� ���������� ��������������� ������
void CNCLearningView::OnTimer(UINT_PTR nIDEvent)
{
	RedrawWindow();				// �������������� ���� �����
	g_eventStart.SetEvent();	// ������������ ���������� ������
	CFormView::OnTimer(nIDEvent);
}

// ������������ ���� � ������� ������ ������� ������ � ����������� ����� �������� 
// � ���������� ����� ���������� �������
// ���������� ���������� �������� ������� �� ���� �������� ����������
// ������������� �� 32 ������� �� ����� minXemImg.dat �������� ������ ����������
// �������� �������, ��������� ��������� �������� �������
void CNCLearningView::OnMorfing()
{
//////////////////////////////////////////////////////////////////////////
// ����� �� ������ �����
  return;
//////////////////////////////////////////////////////////////////////////

	CTestDlg objTest ( NCTEST_MODE_MORFING, TRUE, NULL, NULL, &si, &mixKey, &weightsArr );
	objTest.DoModal();
}

// ����������� ������ ������� ������ ������� �� ���� sPath
// ���������� ������ �������
float CNCLearningView::GetGroupImgWidth(CString sPath)
{
	// ��������� �������� ������ 
	BNCSIGNIMAGE *tstimages;
	tstimages = new BNCSIGNIMAGE[NCIMAGE_COUNT];
	UINT tstimageCount = 0;	
	WCHAR szPath[MAX_PATH];	
	lstrcpy(szPath, objFileManip.tofileFullPath);
	long lErr =	objFileManip.LoadAllImages(szPath, tstimageCount, tstimages);
	if ( lErr != ERROR_SUCCESS ) { 
		objFileManip.DelAllImages(tstimageCount, tstimages); 
		delete [] tstimages;
		return 0.0;
	}
	// ��������� ������������ ����� �������� �������
	FLOAT *coefs;		// ���������� ������ ������������ ���� �������� �������
	// ��������� ������ ��� ������������ ���� �������
	coefs = new FLOAT[tstimageCount*NCFOURIER_COUNT];
	BncGetImgParams (tstimages, tstimageCount, coefs );

	// ��������� ����� �������
	float *averArr = new NCFOURIER;
	float *dispArr = new NCFOURIER;
	float *qualArr = new NCFOURIER;
	fadq ( tstimageCount, NCFOURIER_COUNT, coefs, averArr, dispArr, qualArr );
	float width = faverElem(NCFOURIER_COUNT, dispArr);	// ������ �������
	float step = width/100;								// ������ ����
	float goal = 0;										// ��������� ������ �������
	// ���������� ��������� ��������� ���� �� ������-�����
	NCKEY key1, key2;
	BncCoefsToKey ( averArr, weightsArr, mixKey, key1 );
	int errorCount = 0;
	do 
	{
		goal += step; 
		float *tmpArr = new NCFOURIER;
		for ( int i = 0; i < NCFOURIER_COUNT; i++ )
			tmpArr[i] = averArr[i] + goal;
		BncCoefsToKey ( tmpArr, weightsArr, mixKey, key2 );
		errorCount = BncCompareKeys( key1, key2 );
		delete [] tmpArr;
	} while ( errorCount < 1 );

	//float res = goal/width;

	errorCount = BncCompareKeys( si.key, key2 );

	delete [] averArr;
	delete [] dispArr;
	delete [] qualArr;

	delete [] coefs;

	objFileManip.DelAllImages(tstimageCount, tstimages); 
	delete [] tstimages;
/*
	CString str;
	// d - ������� �������� ��������� ������������ ������,
	// k - ����������� ��������������� ���������,
	// w - ��������� ������ �������
	str.Format(_T("������ ��������������� ������� (������ �������) �����:\n d * k = w\n %.3f * %.3f = %.3f"), width, res, goal);
	MessageBox(str, _T("���������� ���������"), MB_OK);
*/
//	return goal;
	return (float)errorCount;
}

// ���������� ������ ������� "����" � "�����"
void CNCLearningView::OnGrWidth()
{
//////////////////////////////////////////////////////////////////////////
// ����� �� ������ �����
  return;
//////////////////////////////////////////////////////////////////////////

	// 1. ��������� ������ "����"
	// 2. �������� ��������� ���� �� ������� "����"
	// 3. ���������� ����� � ������ (W0) ������� "����"
	// 4. ��������� ������ "�����" (�����-������ ������ "����" � ������ "�����" ������ ���������)
	// 5. ���������� ����� � ������ (W1) ������� "�����"
	// 6. ��������� 1 ����� "�����" �������� �� �����-������ ������ "����"
	// 7. ���������� ������ (W3) ������� "�����", ���� �������� ��� W0 � W3.

// ���� 4 � 5
	// ��� ������, �������� ������
	float widthArr[8];	// ������ i-� ������ �������
	objFileManip.tofileFullPath = _T("D:\\Work\\����\\������_������������\\����������� � ����������� ��������\\����������� ������ �������\\�����_�������_20.dat");
	widthArr[0] = GetGroupImgWidth(objFileManip.tofileFullPath);
	objFileManip.tofileFullPath = _T("D:\\Work\\����\\������_������������\\����������� � ����������� ��������\\����������� ������ �������\\�����_������_20.dat");
	widthArr[1] = GetGroupImgWidth(objFileManip.tofileFullPath);
	objFileManip.tofileFullPath = _T("D:\\Work\\����\\������_������������\\����������� � ����������� ��������\\����������� ������ �������\\�����_���������_20.dat");
	widthArr[2] = GetGroupImgWidth(objFileManip.tofileFullPath);
	objFileManip.tofileFullPath = _T("D:\\Work\\����\\������_������������\\����������� � ����������� ��������\\����������� ������ �������\\�����_�����_14.dat");
	widthArr[3] = GetGroupImgWidth(objFileManip.tofileFullPath);
	objFileManip.tofileFullPath = _T("D:\\Work\\����\\������_������������\\����������� � ����������� ��������\\����������� ������ �������\\�����_�������_12.dat");
	widthArr[4] = GetGroupImgWidth(objFileManip.tofileFullPath);
	objFileManip.tofileFullPath = _T("D:\\Work\\����\\������_������������\\����������� � ����������� ��������\\����������� ������ �������\\�����_������_19.dat");
	widthArr[5] = GetGroupImgWidth(objFileManip.tofileFullPath);
	objFileManip.tofileFullPath = _T("D:\\Work\\����\\������_������������\\����������� � ����������� ��������\\����������� ������ �������\\�����_������_8.dat");
	widthArr[6] = GetGroupImgWidth(objFileManip.tofileFullPath);
	objFileManip.tofileFullPath = _T("D:\\Work\\����\\������_������������\\����������� � ����������� ��������\\����������� ������ �������\\�����_�����_19.dat");
	widthArr[7] = GetGroupImgWidth(objFileManip.tofileFullPath);
	// ��� ������
//	objFileManip.tofileFullPath = _T("D:\\Work\\����\\������_������������\\����������� � ����������� ��������\\����������� ������ �������\\100 ������� ����.dat");
//	widthArr[0] = GetGroupImgWidth(objFileManip.tofileFullPath);
	
//////////////////////////////////////////////////////////////////////////
// ��� ������ (���� 6 � 7)
	int imgNum[] = {1,2,8,29,35,51,54,60,70,80};	// ������ ��������������� ������� �� ����� "100 ������� ����.dat"
	float	goalArr[10];	// ������ i-�� ������
	float	meraArr1[10];
	float	meraArr2[10];
	// ���� � ������ ���� ������, �� ���������, ����� - ���
	if ( imageCount == 0 ) {
		MessageBox(_T("��� ���������� ��������� � ������\n������ ����������� ���� �� ���� �����."), _T("������ ����"), MB_ICONASTERISK);
		return;
	}
	// ��������� ������������ ����� �������� �������
	FLOAT *coefs;		// ���������� ������ ������������ ���� �������� �������
	// ��������� ������ ��� ������������ ���� �������
	coefs = new FLOAT[imageCount*NCFOURIER_COUNT];
	BncGetImgParams (images, imageCount, coefs );
	NCFOURIER averArr;
	for ( int i = 0; i < sizeof(imgNum)/sizeof(imgNum[0]); i++ )
	{
		// ���������� ����� �������
		memcpy(averArr, coefs+imgNum[i]*NCFOURIER_COUNT, sizeof(NCFOURIER));
		float width = faverElem(NCFOURIER_COUNT, averArr);	// ������ �������
		float step = width/1000;							// ������ ����
		goalArr[i] = 0;										// ��������� ������ �������
		NCFOURIER tmpArr;
		// ���������� ��������� ��������� ���� �� ������-�����
		NCKEY key1, key2;
		BncCoefsToKey ( averArr, weightsArr, mixKey, key1 );
		int errorCount = 0;
		do 
		{
			goalArr[i] += step; 
			for ( int j = 0; j < NCFOURIER_COUNT; j++ )
				tmpArr[j] = averArr[j] + goalArr[i];
			BncCoefsToKey ( tmpArr, weightsArr, mixKey, key2 );
			errorCount = BncCompareKeys( key1, key2 );
		} while ( errorCount < 1 );

		for ( int j = 0; j < NCFOURIER_COUNT; j++ )
			tmpArr[j] = averArr[j] + goalArr[i];
		BncCoefsToKey ( tmpArr, weightsArr, mixKey, key2 );
		meraArr1[i] = (float)BncCompareKeys( si.key, key2 );

//		for ( int j = 0; j < NCFOURIER_COUNT; j++ )
//			tmpArr[j] = averArr[j] + widthArr[����� ������];	// ������ ����� ������ �������
//		BncCoefsToKey ( tmpArr, weightsArr, mixKey, key2 );
//		meraArr2[i] = BncCompareKeys( trainKey, key2 );

	}
	float width = faverElem(10, goalArr);	// ������ �������
	float mera1 = faverElem(10, meraArr1);	// ���� 1
	float mera2 = faverElem(10, meraArr2);	// ���� 2
//////////////////////////////////////////////////////////////////////////

	CString str;
	// d - ������� �������� ��������� ������������ ������,
	// k - ����������� ��������������� ���������,
	// w - ��������� ������ �������
//	str.Format(_T("������ ��������������� ������� (������ �������) �����:\n d * k = w\n %.3f * %.3f = %.3f"), width, res, goal);
//	MessageBox(str, _T("���������� ���������"), MB_OK);
	MessageBox(_T("���������� ���������"), _T("���������� ���������"), MB_OK);
//////////////////////////////////////////////////////////////////////////

}
