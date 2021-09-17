// MainFrm.h : interface of the CMainFrame class
//

#pragma once

#include "../Styles/MenuXP.h"

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	
	void SwitchToForm(int nForm);

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
	DECLARE_MENUXP()
public:
	afx_msg void OnManualBtn();
	afx_msg void TrainRegime();
	afx_msg void TestRegime();
	afx_msg void OnExitBtn();
	afx_msg void OnClose( );
	afx_msg void OnSetPswM();
	afx_msg void OnTrainNetM();
	afx_msg void OnTestNetM();
	afx_msg void OnUpdateTrainNet(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTestNet(CCmdUI *pCmdUI);
};


