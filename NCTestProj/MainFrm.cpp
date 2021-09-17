// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "NCTestProj.h"

#include "MainFrm.h"
#include "DlgBarEx.h"
#include "NCTestProjView.h"
#include "NCLearningView.h"
#include "UserAccountDlg.h"

#include "../Tools/FileManipulations.h"
#include "Shlwapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_MENUXP_MESSAGES()
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER,  &CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP,			&CFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, &CFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, &CFrameWnd::OnHelpFinder)
	
	ON_BN_CLICKED(IDC_MANUAL_BTN, &CMainFrame::OnManualBtn)
	ON_BN_CLICKED(IDC_TRAIN_BTN,  &CMainFrame::TrainRegime)
	ON_BN_CLICKED(IDC_TEST_BTN,   &CMainFrame::TestRegime)
	ON_BN_CLICKED(IDC_EXIT_BTN,   &CMainFrame::OnExitBtn)

	ON_COMMAND(ID_SET_PSW,		&CMainFrame::OnSetPswM)
	ON_COMMAND(ID_TRAIN_NET,	&CMainFrame::OnTrainNetM)
	ON_COMMAND(ID_TEST_NET,		&CMainFrame::OnTestNetM)
	ON_UPDATE_COMMAND_UI(ID_TRAIN_NET,	&CMainFrame::OnUpdateTrainNet)
	ON_UPDATE_COMMAND_UI(ID_TEST_NET,	&CMainFrame::OnUpdateTestNet)
END_MESSAGE_MAP()
IMPLEMENT_MENUXP(CMainFrame, CFrameWnd);

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{

}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	if (!m_wndLeftBar.Create(this, IDD_LEFT_BAR,
		CBRS_LEFT|CBRS_TOOLTIPS|CBRS_FLYBY, IDD_LEFT_BAR))
	{
		TRACE0("Failed to create DlgBar\n");
		return -1;      // fail to create
	}

	if (!m_wndRightBar.Create(this, IDD_RIGHT_BAR, 
		CBRS_RIGHT|CBRS_TOOLTIPS|CBRS_FLYBY, IDD_RIGHT_BAR))
	{
		TRACE0("Failed to create DlgBar\n");
		return -1;      // fail to create
	}
	m_wndRightBar.mode = -1;
	if (!m_wndRightBar.CreateListBox(m_wndRightBar.m_ImageList, 2, IDC_IMAGE_LIST, 70))
	{
		TRACE0("Failed to create list box\n");
		return -1;      // fail to create
	}
	RECT pos = {10, 280, 112, 305};
	if (!m_wndRightBar.CreateButton(m_wndRightBar.m_DelImageBtn, 3, IDC_DELIMAGE_BTN, pos, _T("Delete image")))
	{
		TRACE0("Failed to create button\n");
		return -1;      // fail to create
	}
	pos.top  += 30;
	pos.bottom += 30;
	if (!m_wndRightBar.CreateButton(m_wndRightBar.m_DelAllImagesBtn, 4, IDC_CLEARLIST_BTN, pos, _T("clear the list")))
	{
		TRACE0("Failed to create button\n");
		return -1;      // fail to create
	}
	// Создание кнопочки "Добавить"
	pos.top  = 365;
	pos.bottom = 390;
	if (!m_wndRightBar.CreateButton(m_wndRightBar.m_AddImageBtn, 5, IDC_ADD_BTN, pos, _T("Add image")))
	{
		TRACE0("Failed to create button\n");
		return -1;      // fail to create
	}
	// Создание кнопочки "Обучить"
	{
		pos.top += 30;
		pos.bottom += 30;
		if (!m_wndRightBar.CreateButton(m_wndRightBar.m_LearnNetBtn, 6, IDC_LEARN_BTN, pos, _T("Train the network")))
		{
			TRACE0("Failed to create button\n");
			return -1;      // fail to create
		}
		m_wndRightBar.ShowWindow(SW_HIDE);
	}

	if (!m_wndBotDlgBar.Create(this, IDD_BOTTOM_BAR,
		CBRS_BOTTOM|CBRS_TOOLTIPS|CBRS_FLYBY, IDD_BOTTOM_BAR))
	{
		TRACE0("Failed to create DlgBar\n");
		return -1;      // fail to create
	}
	// Создание кнопочки "Очистить"
	pos.left   = 190;
	pos.top    = 4;
	pos.right  = pos.left + 120;
	pos.bottom = pos.top + 25;
	
	if (!m_wndBotDlgBar.CreateButton(m_wndBotDlgBar.m_ClearBtn, 7, IDC_CLEAR_BTN, pos, _T("Clear input field")))
	{
		TRACE0("Failed to create button\n");
		return -1;      // fail to create
	}
	// Создание кнопочки "Проверить"
	pos.left  = pos.right + 10;
	pos.right = pos.left + 120;
	if (!m_wndBotDlgBar.CreateButton(m_wndBotDlgBar.m_CheckBtn, 8, IDC_CHECK_BTN, pos, _T("Verify")))
	{
		TRACE0("Failed to create button\n");
		return -1;      // fail to create
	}
	m_wndBotDlgBar.ShowWindow(SW_HIDE);

	// Comment this line to deactivate the XP Look & Feel
	CMenuXP::SetXPLookNFeel (this);
	CMenuXP::UpdateMenuBar (this);

	// Вычисление полного пути	
	TCHAR cpath[MAX_PATH];
	GetModuleFileName(0, cpath, sizeof(cpath));
	PathRemoveFileSpec(cpath);
	objFileManip.fullPath = cpath;
	objFileManip.fullPath += "\\";

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// Size the window and center it
	cs.cx = 800; 
	cs.cy = 550;
	cs.y = (::GetSystemMetrics(SM_CYSCREEN) - cs.cy) / 2;
	cs.x = (::GetSystemMetrics(SM_CXSCREEN) - cs.cx) / 2;
	
	return CFrameWnd::PreCreateWindow(cs);
}
void CMainFrame::SwitchToForm(int nForm)
{
	CView* pOldActiveView = GetActiveView();
	CView* pNewActiveView = (CView*)GetDlgItem(nForm);
	if (pNewActiveView == NULL)
	{
		switch(nForm)
		{
		case IDD_FIRST_FORM:  	pNewActiveView = (CView*)new CNCTestProjView; break;
		case IDD_TRAIN_FORM:	pNewActiveView = (CView*)new CNCLearningView; break;
		}

		CCreateContext context;
		context.m_pCurrentDoc = pOldActiveView->GetDocument();
		pNewActiveView->Create(NULL, NULL, 0L, CFrameWnd::rectDefault,
			this, nForm, &context);
//		pNewActiveView->OnInitialUpdate();
	}

	SetActiveView(pNewActiveView);
	pNewActiveView->ShowWindow(SW_SHOW);
	pOldActiveView->ShowWindow(SW_HIDE);
	pOldActiveView->SetDlgCtrlID(
		pOldActiveView->GetRuntimeClass() == 
		RUNTIME_CLASS(CNCTestProjView) ? IDD_FIRST_FORM : IDD_TRAIN_FORM);
	pNewActiveView->SetDlgCtrlID(AFX_IDW_PANE_FIRST);
	RecalcLayout();

}

// CMainFrame diagnostics

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


// CMainFrame message handlers

// Режим создания пользовательского пароля
void CMainFrame::OnSetPswM()
{
	CUserAccountDlg objDlg;
	objDlg.DoModal();
}

// Открыть инструкцию пользователя
void CMainFrame::OnManualBtn()
{
	// Задаём путь к инструкции пользователя	
	objFileManip.tofileFullPath = objFileManip.fullPath + _T("Manual.htm");

	// Открываем инструкцию пользователя
	DWORD dwStatus = 
			(DWORD)ShellExecute ( NULL, NULL, 
								  objFileManip.tofileFullPath, 
								  NULL, NULL, SW_SHOWNORMAL );
	// Определяем наличие ошибок
	if (dwStatus < 32) 
	{
		CString sMessage;
		switch(dwStatus)
		{
		case ERROR_FILE_NOT_FOUND:       sMessage = " Файл не найден "; break;
		case ERROR_PATH_NOT_FOUND:       sMessage = " Путь не найден "; break;
		default:                         sMessage = " Невозможно открыть инструкцию "; 
		}
		MessageBox (sMessage, _T("Ошибка при открывании"), MB_ICONWARNING);
	}
}

// Режим обучения нейронной сети
void CMainFrame::TrainRegime()
{
	if ( !GetActiveView()->IsKindOf(RUNTIME_CLASS(CNCLearningView)) ) {
		SwitchToForm(IDD_TRAIN_FORM);
	}

	if ( m_wndRightBar.mode == NCTRAIN_MODE )
    return;

	m_wndRightBar.mode = NCTRAIN_MODE;
	CView* pActiveView = GetActiveView();
	pActiveView->OnInitialUpdate();
	SetWindowText(_T("BioNeuroAutograph 2.0 - training mode"));
	m_wndRightBar.ShowWindow(SW_SHOW);
	m_wndBotDlgBar.ShowWindow(SW_SHOW);
		
	RecalcLayout();
		
	m_wndRightBar.OnInitDialog();
	m_wndBotDlgBar.OnInitDialog();

  RedrawWindow();
}
void CMainFrame::OnTrainNetM()
{
	TrainRegime();
}
void CMainFrame::OnUpdateTrainNet(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_wndRightBar.mode == NCTRAIN_MODE );
}

// Режим тестирования нейронной сети
void CMainFrame::TestRegime()
{
	if ( !GetActiveView()->IsKindOf(RUNTIME_CLASS(CNCLearningView)) ) {
		SwitchToForm(IDD_TRAIN_FORM);
	}
	
	if ( m_wndRightBar.mode != NCTEST_MODE ) {
		m_wndRightBar.mode = NCTEST_MODE;
		CView* pActiveView = GetActiveView();
		pActiveView->OnInitialUpdate();
		SetWindowText(_T("BioNeuroAutograph 2.0 - testing mode"));
		m_wndRightBar.ShowWindow (SW_SHOW);
		m_wndBotDlgBar.ShowWindow(SW_SHOW);

		RecalcLayout();

		m_wndRightBar.OnInitDialog();
		m_wndBotDlgBar.OnInitDialog();

		RedrawWindow();
	}
}
void CMainFrame::OnTestNetM()
{
	TestRegime();
}
void CMainFrame::OnUpdateTestNet(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_wndRightBar.mode == NCTEST_MODE );
}

// Выход из программы
void CMainFrame::OnClose()
{
	if ( MessageBox(_T("Quit working with the BioNeuroAutograph program?"), _T("Output"), MB_ICONQUESTION | MB_YESNO) == IDYES )
		CFrameWnd::OnClose();
}
void CMainFrame::OnExitBtn()
{
	OnClose();
}
